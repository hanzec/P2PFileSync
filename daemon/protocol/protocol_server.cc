#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <glog/logging.h>
#include <glog/vlog_is_on.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <regex>
#include <string>

#include "common.pb.h"
#include "hello_message.pb.h"
#include "ip_addr.h"
#include "log.h"
#include "message.pb.h"
#include "protocol.h"
#include "protocol/utils/fifo_cache.h"
#include "utils/thread_pool.h"

namespace P2PFileSync::Protocol {
// get instance of ProtocolServer object
std::shared_ptr<ProtocolServer> ProtocolServer::get_instance() {
  return _instance;
};

// initlized protol instance
bool ProtocolServer::init(const Config &config,
                          Serverkit::DeviceContext &device_context) {
  // prevent init twice
  if (_instance != nullptr) return true;

  // will constantly block until client is activated
  while (!device_context.is_enabled()) {
    sleep(60);  // wait for 1 minutes and check activation status again
  }

  // creating socket
  int socket_fd;
  {
    // build the socket for server
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0) < 0)) {
      LOG(FATAL) << "socket cannot open";
    }

    // bind the socket to address
    struct sockaddr_in adr_s;
    adr_s.sin_family = AF_INET;
    adr_s.sin_port = htons(config.get_trans_port_number());

    // if defined ip address will only listen on that specific address otherwise
    // will listen to 0.0.0.0
    if (config.get_listen_ip_address().empty()) {
      adr_s.sin_addr.s_addr = inet_addr("0.0.0.0");
    } else {
      adr_s.sin_addr.s_addr = inet_addr(config.get_listen_ip_address().c_str());
    }

    // bind the socket to listen address
    if (bind(socket_fd, (struct sockaddr *)&adr_s, sizeof(adr_s)) == -1) {
      close(socket_fd);
      LOG(FATAL) << "cannot bind socket to interface!";
    }

    // listen this address
    listen(socket_fd, 20);
  }

  // creating instance
  _device_context = device_context;
  _instance = std::make_shared<ProtocolServer>(
      config.get_workder_thread_num(), socket_fd,
      device_context.get_certificate(),
      device_context.get_server_sign_certificate(),
      config.get_packet_cache_size());

  return true;
};

ProtocolServer::ProtocolServer(const uint8_t number_worker, const int fd,
                               const PKCS12 *cert, const PKCS7 *cert_sign,
                               const uint32_t packet_size)
    : FIFOCache(packet_size),    // initial packet cache
      ThreadPool(number_worker), // initial thread pool
      _sign(cert_sign),
      _certificate(cert),
      _thread_ref(std::thread(listening_thread, fd, this)) {
  VLOG(INFO) << "Initialized instance of protocol server";

  // initalized base_event
  if ((_event_base = event_base_new()) == nullptr)
    LOG(FATAL) << "libevent base event create failed!";

  // send hello message to all clients, client will add node when response get
  for (const auto &ip : _device_context.get_peer_list()) {
    this->send_hello(IPAddr(ip.second));
  }
};



void ProtocolServer::read_callback(struct bufferevent *bev, void *ctx) {
  struct evbuffer *buf = bufferevent_get_input(bev);
  auto proto_server = static_cast<ProtocolServer *>(ctx);

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
      VLOG(3) << "cannot found desnation for receiver ["
              << message.receiver_id() << "]";
      return;
    }

    // redirect packet to destination
    proto_server->send_raw_package(
        data, packen_len + sizeof(uint32_t),
        proto_server->get_ip_addr_from_table(message.receiver_id()));
  }

  // handles the request
  proto_server->handle_package_async(message);
}

void ProtocolServer::event_callback(struct bufferevent *bev, short events, void *ctx) {
  struct info *inf = ctx;
  struct evbuffer *input = bufferevent_get_input(bev);
  int finished = 0;

  if (events & BEV_EVENT_EOF) {
    size_t len = evbuffer_get_length(input);
    printf(
        "Got a close from %s.  We drained %lu bytes from it, "
        "and have %lu left.\n",
        inf->name, (unsigned long)inf->total_drained, (unsigned long)len);
    finished = 1;
  }
  if (events & BEV_EVENT_ERROR) {
    printf("Got an error from %s: %s\n", inf->name,
           evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
    finished = 1;
  }
  if (finished) {
    free(ctx);
    bufferevent_free(bev);
  }
}

void ProtocolServer::libev_callback(evutil_socket_t fd, short event, void *arg) {
  struct sockaddr_in sin;
  socklen_t slen = sizeof(sin);
  auto proto_server = static_cast<ProtocolServer *>(arg);

  // waiting for new connection accepted
  int new_fd;
  if ((new_fd = accept(fd, (struct sockaddr *)&sin, (socklen_t *)&slen)) < 0) {
    LOG(ERROR) << "Error when handling incoming connection";
    return;
  } else {
    VLOG(3) << "Handle incoming connection for fd:[" << fd << "]";
  }

  auto *bev = bufferevent_socket_new(proto_server->get_event_base(), new_fd, BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb(bev, read_callback, nullptr, event_callback, arg);
  bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
}

void ProtocolServer::listening_thread(int fd, ProtocolServer &protool_server) {
  auto *listen_ev =
      event_new(protool_server.get_event_base(), fd, EV_READ | EV_PERSIST,
                libev_callback, (void *)&protool_server);

  event_add(listen_ev, nullptr);
  event_base_dispatch(protool_server.get_event_base());
  event_del(listen_ev);
  event_base_free(protool_server.get_event_base());
};

ProtocolServer::~ProtocolServer() { close(s); }

}  // namespace P2PFileSync::Protocol