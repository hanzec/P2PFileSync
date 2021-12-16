#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <glog/logging.h>
#include <google/protobuf/util/time_util.h>
#include <openssl/err.h>
#include <protocol.pb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <string>

#include "p2p_interface.h"
#include "packet/tasks.h"
#include "utils/data_struct/fifo_cache.h"
#include "utils/data_struct/thread_pool.h"
#include "utils/ip_addr.h"
#include "utils/uuid_utils.h"

#ifdef UNDER_UNIX
#include <cerrno>
#endif
namespace P2PFileSync {

void P2PServerContext::block_util_server_stop() {
  if (_thread_ref->joinable()) {
    _thread_ref->join();
  }
}

// get instance of P2PServerContext object
std::shared_ptr<P2PServerContext> P2PServerContext::get_instance() {
  if (_instance == nullptr) {
    return nullptr;
  } else {
    return _instance;
  }
};

const std::unordered_map<std::string, std::pair<std::shared_ptr<IPAddr>, uint32_t>>
    &P2PServerContext::get_online_peers() {
  return get_routing_table();
}

bool P2PServerContext::start(int fd) {
  if (_thread_ref == nullptr) {
    auto hello_message = construct_payload<ProtoHelloMessage>();
    _thread_ref = std::make_unique<std::thread>(listening_thread, fd);
    auto peer_list_resp = _device_context->peer_list();
    for (const auto &ip : peer_list_resp->peer_list()) {
      VLOG(3) << "send hello message to " << ip.first << ":" << ip.second;
      auto ip_addr = std::make_shared<IPAddr>(ip.second);
      send_pkg(package_pkg<ProtoHelloMessage>(hello_message, ip.first), ip_addr);
    }
    return true;
  } else {
    return false;
  }
}

// initlized protol instance
bool P2PServerContext::init(const std::shared_ptr<Config> &config,
                            std::shared_ptr<ThreadPool> thread_pool,
                            const std::shared_ptr<ServerKit::DeviceContext> &device_context) {
  // prevent init twice
  if (_instance != nullptr) return false;

  // will constantly block until client is activated
  while (!device_context->is_enabled()) {
    LOG(ERROR) << "current device is not enabled, will check activate status in 1 minutes "
                  "later";
    sleep(60);  // wait for 1 minutes and check activation status again
  }

  // creating socket
  int socket_fd = -1;
  {
    // build the socket for server
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
#ifdef UNDER_UNIX
      LOG(ERROR) << "create socket failed, errno:" << errno << "(" << strerror(errno) << ")";
#else
      LOG(ERROR) << "socket cannot open";
#endif
      return false;
    }

    // bind the socket to address
    struct sockaddr_in adr_s {
      0
    };
    adr_s.sin_family = AF_INET;
    adr_s.sin_port = htons(config->p2p_port());

    // if defined ip address will only listen on that specific address otherwise
    // will listen to 0.0.0.0
    if (config->p2p_listen_address().empty()) {
      adr_s.sin_addr.s_addr = INADDR_ANY;
      LOG(WARNING) << "not found listing address in config file, will listen 0.0.0.0";
    } else {
      adr_s.sin_addr.s_addr = inet_addr(config->p2p_listen_address().c_str());
    }

    // bind the socket to listen address
    if (bind(socket_fd, (struct sockaddr *)&adr_s, sizeof(adr_s)) != 0) {
#ifdef UNDER_UNIX
      LOG(ERROR) << "cannot bind socket to interface, errno:" << errno << "("
                 << strerror(errno) << ")";
#else
      LOG(ERROR) << "cannot bind socket to interface";
#endif
      close(socket_fd);
      return false;
    }

    // listen this address
    if (0 > listen(socket_fd, 20)) {
#ifdef UNDER_UNIX
      LOG(ERROR) << "cannot listen on socket, errno:" << errno << "(" << strerror(errno)
                 << ")";
#else
      LOG(ERROR) << "cannot listen on socket";
#endif
      close(socket_fd);
      return false;
    }
  }

  // loading PKCS12 certificate
  PKCS12 *cert;
  X509 *client_cert = nullptr;
  EVP_PKEY *client_priv_key = nullptr;
  STACK_OF(X509) *sign_chain = nullptr;

  // loading certificate from disk
  FILE *file = fopen(device_context->client_certificate().c_str(), "rb");
  if ((cert = d2i_PKCS12_fp(file, nullptr)) == nullptr) {
    LOG(ERROR) << "unable to loading [" << device_context->client_certificate() << "]!";
    return false;
  } else {
    LOG(INFO) << "success loading client certificate to memory!";
  }

  // unpacking client certificate from PCSC12
  if (!PKCS12_parse(cert, device_context->device_id().c_str(), &client_priv_key, &client_cert,
                    &sign_chain)) {
    LOG(ERROR) << "Error parsing PKCS#12 file\n";
    return false;
  }

  // creating instance
  _device_context = device_context;
  _instance = create(config->packet_cache_size(), thread_pool, client_cert, client_priv_key,
                     config->p2p_port(), sign_chain);

  VLOG(3) << "P2PServerContext init success";
  return _instance->start(socket_fd);
};

P2PServerContext::P2PServerContext(const this_is_private &, uint32_t packet_cache_size,
                                   std::shared_ptr<ThreadPool> &thread_pool, X509 *cert,
                                   EVP_PKEY *private_key, uint16_t port,
                                   STACK_OF(X509) * ca)
    : FIFOCache(packet_cache_size),  // initial packet cache
      _port(port),
      _client_cert(cert),
      _thread_pool(thread_pool),
      _x509_store(X509_STORE_new()),
      _client_priv_key(private_key) {
  LOG(INFO) << "Initialized instance of packet server";

  if (nullptr == (_event_base = event_base_new())) {
    LOG(FATAL) << "cannot initialize event base";
  }

  // loading CA certificate
  for (int i = 0; i < sk_X509_num(ca); i++) {
    X509 *x509 = sk_X509_value(ca, i);
    X509_STORE_add_cert(_x509_store, x509);
  }

  /*
   * Tyring to verify the client certificate
   */
  X509_STORE_CTX *_x509_store_ctx = X509_STORE_CTX_new();

  // openssl related structure
  X509_STORE_set_flags(_x509_store, 0);
  if (X509_STORE_CTX_init(_x509_store_ctx, _x509_store, cert, ca) != 1) {
    LOG(FATAL) << "cannot initialize x509 store context";
  }

  if (X509_verify_cert(_x509_store_ctx) != 1) {
    X509_STORE_CTX_free(_x509_store_ctx);
    LOG(FATAL) << "cannot verify client certificate";
  } else {
    X509_STORE_CTX_free(_x509_store_ctx);
    X509_STORE_add_cert(_x509_store, cert);
  }
};

void P2PServerContext::listening_thread(int fd) {
  auto proto = get_instance();

  // initialized base_event
  if (nullptr == proto->_event_base) LOG(FATAL) << "libevent base event create failed!";

  auto *ev = event_new(proto->event_base(), fd, EV_READ | EV_PERSIST, libev_callback, nullptr);
  event_add(ev, nullptr);
  event_base_dispatch(proto->event_base());
  event_del(ev);
  event_base_free(proto->event_base());
};

void P2PServerContext::libev_callback(evutil_socket_t fd, short event, void *arg) {
  int new_fd = 0;
  struct sockaddr_in sin {
    0
  };
  socklen_t size = sizeof(sin);
  if ((new_fd = accept(fd, (struct sockaddr *)&sin, (socklen_t *)&size)) < 0) {
    LOG(ERROR) << "Error when handling incoming connection";
    return;
  } else {
    VLOG(3) << "Handle incoming connection for fd:[" << fd << "]";
  }

  auto *bev =
      bufferevent_socket_new(get_instance()->event_base(), new_fd, BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb(bev, read_callback, nullptr, event_callback, &sin);
  bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
}

void P2PServerContext::read_callback(struct bufferevent *bev, void *sin) {
  auto proto_server = get_instance();
  struct evbuffer *buf = bufferevent_get_input(bev);
  auto incoming_connection = static_cast<struct sockaddr_in *>(sin);

  // first 4 bytes of packet is defined as packet length
  size_t recv_size = evbuffer_get_length(buf);
  if (recv_size < sizeof(uint32_t)) return;

  // check if we received full data of the packet
  uint32_t packet_len;
  if (evbuffer_copyout(buf, &packet_len, sizeof(uint32_t)) != sizeof(uint32_t))
    LOG(FATAL) << "read data from evbuffer failed!";
  if (recv_size - sizeof(uint32_t) < packet_len) return;

  // handling incoming packet
  LOG(INFO) << "Received packet of size:[" << packet_len << "]";

  // parse data packet
  ProtoMessage proto_msg;
  SignedProtoMessage signed_msg;
  void *data = evbuffer_pullup(buf, packet_len + sizeof(uint32_t));
  if (!signed_msg.ParseFromArray((char *)data + sizeof(uint32_t), packet_len)) {
    LOG(ERROR) << "signed_msg failed to parsed";
  }

  // abandon all packet with ttl == 0
  if (signed_msg.ttl() == 0) return;

  // todo update rounting table if hello message
  // avoid the handle_difficult one packet twice
  proto_msg.ParseFromString(signed_msg.signed_payload());
  if (proto_server->is_cached(proto_msg.packet_id())) {
    VLOG(3) << "skip previous handled signed_msg with id [" << proto_msg.packet_id();
    return;
  }

  // todo need to rewrite the packet redirect part
  // check if the package's final deliver is current peer or not
  auto receiver = proto_msg.receiver_id();
  if (proto_server->_device_context->device_id() != receiver) {
    // redirect packet if ttl > 1 and have avaliable path
    if (signed_msg.ttl() > 1 && proto_server->can_delivered(receiver)) {
      proto_server->_thread_pool->submit(Task::send_packet_tcp, _instance->_port, signed_msg,
                                         proto_server->get_next_peer(receiver));
    } else {
      VLOG(3) << "cannot found desnation for receiver [" << proto_msg.receiver_id() << "]";
      return;
    }
  }

  // handle packet by packet type
  switch (proto_msg.packet_type()) {
    case ProtoPayloadType::HELLO: {
      // only hello message won't include signature in outside since handshake
      ProtoHelloMessage msg;
      proto_msg.payload().UnpackTo(&msg);
      proto_server->_thread_pool->submit(handle_complicated<ProtoHelloMessage>, get_instance(),msg,signed_msg.prev_jump_port(),
                                         incoming_connection, signed_msg.ttl());
    } break;
    case ProtoPayloadType::PING:

      break;
    case ProtoPayloadType::PONG:
      break;
    case ProtoPayloadType::ACK:
      break;
    case ProtoPayloadType::NACK:
      break;
    case ProtoPayloadType::HEARTBEAT:
      break;
    default:
      LOG(ERROR) << "unknown packet type [" << proto_msg.packet_type() << "]";
      break;
  }
}

void P2PServerContext::event_callback(struct bufferevent *bev, short events, void *ctx) {
  struct evbuffer *input = bufferevent_get_input(bev);
  int finished = 0;

  if (events & BEV_EVENT_EOF) {
    size_t len = evbuffer_get_length(input);
    if (len != 0)
      LOG(ERROR) << "Received EOF with [" << (unsigned long)len << "] bytes unread! ";
    finished = 1;
  }

  if (events & BEV_EVENT_ERROR) {
    LOG(ERROR) << evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR());
    finished = 1;
  }

  if (finished) {
    bufferevent_free(bev);
  }
}

std::future<bool> P2PServerContext::send_pkg(const ProtoMessage &data,
                                             const std::shared_ptr<IPAddr> &peer) {
  size_t digest_len;
  auto raw_packet_size = data.ByteSizeLong();
  auto raw_packet = std::malloc(raw_packet_size);
  data.SerializeToArray(raw_packet, raw_packet_size);

  if (1 != EVP_DigestSignInit(_evp_md_ctx, nullptr, EVP_sha256(), nullptr, _client_priv_key)) {
    LOG(FATAL) << "EVP_DigestVerifyInit failed, error" << std::hex << ERR_get_error();
  }

  // use the openssl to sign the packet
  if (1 != EVP_DigestSignUpdate(_evp_md_ctx, raw_packet, raw_packet_size)) {
    LOG(FATAL) << "EVP_DigestVerifyUpdate failed, error" << std::hex << ERR_get_error();
  }

  if (1 != EVP_DigestSignFinal(_evp_md_ctx, nullptr, &digest_len)) {
    LOG(ERROR) << "EVP_DigestSignFinal failed, error" << std::hex << ERR_get_error();
    return std::async(std::launch::deferred, []() { return false; });
  }

  unsigned char digest[digest_len];
  if (1 != EVP_DigestSignFinal(_evp_md_ctx, digest, &digest_len)) {
    LOG(ERROR) << "EVP_DigestSignFinal failed, error" << std::hex << ERR_get_error();
    return std::async(std::launch::deferred, []() { return false; });
  }

  //  if (0 >= EVP_DigestSign(_evp_md_ctx, digest, &digest_len, (unsigned char *)raw_packet,
  //                          raw_packet_size)) {
  //    LOG(ERROR) << "sign failed";
  //  }

  // prepare final message
  SignedProtoMessage signed_msg;
  // TODO will use user-defined value later, right now for keeping things simple will be set to
  // 10
  signed_msg.set_ttl(10);
  signed_msg.set_signature(digest, digest_len);
  signed_msg.set_sign_algorithm("SHA256");
  signed_msg.set_signed_payload(raw_packet, raw_packet_size);

  free(raw_packet);
  VLOG(3) << "send packet with id [" << data.packet_id() << "] to [" << *peer << "]";
  return _thread_pool->submit(Task::send_packet_tcp, _port, signed_msg, peer);
}

template <typename T>
ProtoMessage P2PServerContext::package_pkg(const T &data, const std::string &receiver) {
  ProtoMessage ret;
  auto *timestamp =
      new google::protobuf::Timestamp(google::protobuf::util::TimeUtil::GetCurrentTime());

  // static assert proto message type
  static_assert(std::is_base_of<google::protobuf::Message, T>::value,
                "ProtoMessage must be derived from google::protobuf::Message");

  // set packet type
  if constexpr (std::is_same<T, ProtoHelloMessage>::value) {
    ret.set_packet_type(ProtoPayloadType::HELLO);
  }

  auto packet_id = UUID::new_uuid();
  ret.set_packet_id(packet_id);
  ret.mutable_payload()->PackFrom(data);
  ret.set_allocated_timestamp(timestamp);  // set allocated wil take pointer's ownership
  ret.set_receiver_id(receiver);
  ret.set_sender_id(_device_context->device_id());
  free((void *)packet_id);
  VLOG(3) << "package packet with id [" << ret.packet_id() << "]";
  return ret;
}

}  // namespace P2PFileSync