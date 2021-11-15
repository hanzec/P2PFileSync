#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <glog/logging.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <regex>
#include <string>

#include "ip_addr.h"
#include "log.h"
#include "protocol.h"
#include "protolcol.pb.h"
#include "utils/fifo_cache.h"
#include "utils/thread_pool.h"

namespace P2PFileSync::Protocol {
// get instance of ProtocolServer object
std::shared_ptr<ProtocolServer> ProtocolServer::get_instance() { return _instance; };

// initlized protol instance
bool ProtocolServer::init(const Config &config,
                          const std::shared_ptr<Serverkit::DeviceContext> &device_context) {
  // prevent init twice
  if (_instance != nullptr) return false;

  // will constantly block until client is activated
  while (!device_context->is_enabled()) {
    sleep(60);  // wait for 1 minutes and check activation status again
  }

  // creating socket
  int socket_fd;
  {
    // build the socket for server
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0) < 0)) {
      LOG(ERROR) << "socket cannot open";
      return false;
    }

    // bind the socket to address
    struct sockaddr_in adr_s {};
    adr_s.sin_family = AF_INET;
    adr_s.sin_port = htons(config.get_trans_port_number());

    // if defined ip address will only listen on that specific address otherwise
    // will listen to 0.0.0.0
    if (config.get_listen_ip_address().empty()) {
      adr_s.sin_addr.s_addr = inet_addr("0.0.0.0");
      LOG(WARNING) << "not found listing address in config file, will listen 0.0.0.0";
    } else {
      adr_s.sin_addr.s_addr = inet_addr(config.get_listen_ip_address().c_str());
    }

    // bind the socket to listen address
    if (bind(socket_fd, (struct sockaddr *)&adr_s, sizeof(adr_s)) == -1) {
      close(socket_fd);
      LOG(ERROR) << "cannot bind socket to interface!";
      return false;
    }

    // listen this address
    listen(socket_fd, 20);
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
  if (!PKCS12_parse(cert, nullptr, &client_priv_key, &client_cert, &sign_chain)) {
    LOG(ERROR) << "Error parsing PKCS#12 file\n";
    return false;
  }

  // creating instance
  _device_context = device_context;
  _instance = std::shared_ptr<ProtocolServer>(new ProtocolServer(
      config.get_workder_thread_num(), socket_fd, config.get_packet_cache_size(), client_cert,
      client_priv_key, sign_chain));
};

ProtocolServer::ProtocolServer(uint8_t number_worker, int fd, int32_t packet_cache_size,
                               X509 *cert, EVP_PKEY *private_key, STACK_OF(X509) * ca)
    : FIFOCache(packet_cache_size),     // initial packet cache
      ThreadPool(number_worker),  // initial thread pool
      _client_cert(cert),
      _client_priv_key(private_key),
      _sign_chain(ca),
      _thread_ref(std::thread(listening_thread, fd)) {
  VLOG(INFO) << "Initialized instance of protocol server";

  // initalized base_event
  if ((_event_base = event_base_new()) == nullptr)
    LOG(FATAL) << "libevent base event create failed!";

  // send hello message to all clients, client will add node when response get
  for (const auto &ip : _device_context->peer_list()) {
    this->send_hello(IPAddr(ip.second));
  }
};

void ProtocolServer::handle(const ProtoMessage &message,
                            struct sockaddr_in *incoming_connection) {
  switch (message.request_type()) {
    case ProtoPayloadType::HELLO_MESSAGE:
      ProtoHelloMessage msg;
      submit(handle_packet, _peer_pool.get_instance(message.sender_id()),message.payload());
      break;
    case REDIRECT_MESSAGE:
      break;
    default:
      break;
  }
}

void ProtocolServer::listening_thread(int fd) {
  auto proto = get_instance();
  auto *ev = event_new(proto->event_base(), fd, EV_READ | EV_PERSIST, libev_callback, nullptr);
  event_add(ev, nullptr);
  event_base_dispatch(proto->event_base());
  event_del(ev);
  event_base_free(proto->event_base());
};

void ProtocolServer::libev_callback(evutil_socket_t fd, short event, void *arg) {
  int new_fd = 0;
  socklen_t size = sizeof(struct sockaddr_in);
  auto sin = static_cast<struct sockaddr_in *>(malloc(size));
  if ((new_fd = accept(fd, (struct sockaddr *)&sin, (socklen_t *)&size)) < 0) {
    LOG(ERROR) << "Error when handling incoming connection";
    return;
  } else {
    VLOG(3) << "Handle incoming connection for fd:[" << fd << "]";
  }

  auto *bev =
      bufferevent_socket_new(get_instance()->event_base(), new_fd, BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb(bev, read_callback, nullptr, event_callback, sin);
  bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
}

void ProtocolServer::read_callback(struct bufferevent *bev, void *sin) {
  auto proto_server = get_instance();
  struct evbuffer *buf = bufferevent_get_input(bev);
  auto incoming_connection = static_cast<struct sockaddr_in *>(sin);

  // first 4 bytes of packet is defined as packet length
  size_t recv_size = evbuffer_get_length(buf);
  if (recv_size < sizeof(uint32_t)) return;

  // check if we received full data of the packet
  ssize_t packen_len;
  if (evbuffer_copyout(buf, &packen_len, sizeof(uint32_t)) != packen_len)
    LOG(FATAL) << "read data from evbuffer failed!";
  if (recv_size - sizeof(uint32_t) < packen_len) return;

  // parse data packet
  ProtoMessage message;
  void *data = evbuffer_pullup(buf, packen_len + sizeof(uint32_t));
  if (!message.ParseFromArray((char *)data + sizeof(uint32_t), packen_len)) {
    LOG(ERROR) << "message failed to parsed";
  }

  // avoid the handle one packet twice
  if (proto_server->is_cached(message.packet_id())) {
    VLOG(3) << "skip previous handled message with id [" << message.packet_id();
    return;
  }

  // check if the package's final deliever is current peer or not
  if (message.receiver_id() == proto_server->get_peer_id()) {
    // checking of the packet can deliever to desnation peer or not
    if (proto_server->in_routing_table(message.receiver_id())) {
      VLOG(3) << "cannot found desnation for receiver [" << message.receiver_id() << "]";
      return;
    }

    // redirect packet to destination
    proto_server->send_raw_package(
        data, packen_len + sizeof(uint32_t),
        proto_server->get_ip_addr_from_table(message.receiver_id()));
  }

  // handles the request
  proto_server->handle(message, incoming_connection);
}

void ProtocolServer::event_callback(struct bufferevent *bev, short events, void *ctx) {
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
    free(ctx);
    bufferevent_free(bev);
  }
}

}  // namespace P2PFileSync::Protocol