
#include "manage_interface.h"

#include <event.h>
#include <event2/bufferevent.h>
#include <fcntl.h>
#include <glog/logging.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstddef>
#include <cstring>
#include <memory>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <utility>

#include "interface/command_factory.h"
#include "interface/connection_container.h"
#include "interface/utils/command_parser.h"
#include "utils/log.h"
#include "utils/ip_address.h"

namespace P2PFileSync {
ManagementInterface::ManagementInterface(const ManagementInterface::this_is_private &,
                                         const std::string &socket_addr, int listen_type,
                                         int timeout) {
  VLOG(INFO) << "starting manage thread listen on [" << socket_addr << "]";

  if (socket_addr.length() > 108) {
    LOG(FATAL) << "socket file path cannot be longer than 108 character";
  }

  int socket_fd = -1;
  // set up file descriptor
  if ((socket_fd = socket(listen_type, SOCK_STREAM, 0)) == -1) {
    LOG(FATAL) << "can not create socket: " << strerror(errno);
  } else {
    VLOG(3) << "create socket success, socket fd: " << socket_fd;
  }

  /*
   * @Note: have to use sockaddr_storage for having enough space to store and have to use
   * actual size because wrong size could make bind filed with "address already in use" error
   */
  size_t actual_size = -1;
  struct sockaddr_storage addr {};
  if (listen_type == AF_UNIX) {  // unix socket
    unlink(socket_addr.c_str());
    actual_size = sizeof(struct sockaddr_un);
    auto *addr_un = (struct sockaddr_un *)&addr;
    addr_un->sun_family = listen_type;
    memcpy(addr_un->sun_path, socket_addr.c_str(), socket_addr.length());
    VLOG(3) << "socket is AF_UNIT and socket path is [" << addr_un->sun_path << "]";
  } else if (listen_type == AF_INET) {  // ipv4 socket
    IPAddress ip_addr(socket_addr);
    actual_size = sizeof(struct sockaddr_in);
    auto *addr_in = (struct sockaddr_in *)&addr;
    addr_in->sin_family = listen_type;
    addr_in->sin_port = htons(ip_addr.port());
    inet_aton(ip_addr.ip_address().c_str(), &addr_in->sin_addr);
    VLOG(3) << "socket is AF_INET and ip is: " << ip_addr.ip_address()
            << " and port is: " << ip_addr.port();
  } else {
    LOG(FATAL) << "unsupported listen type";
  }

  // bind socket between socket and file descriptor
  if (bind(socket_fd, (struct sockaddr *)&addr, actual_size) == -1) {
    LOG(FATAL) << "cannot bind open socket to [" << socket_addr << "]: " << strerror(errno);
  } else {
    VLOG(3) << "bind socket to [" << socket_addr << "] success";
  }

  // connect
  _manage_interface_thread =
      std::thread(&ManagementInterface::manage_interface_thread, socket_fd);
}

void ManagementInterface::join() { _manage_interface_thread.join(); }

void ManagementInterface::manage_interface_thread(int listen_fd) {
  // connect
  if (listen(listen_fd, 10) < 0) { /* tell kernel we're a server */
    LOG(FATAL) << "cannot set socket [" << listen_fd << "] status to listen";
  }

  // libevent
  get()->_event_base = event_base_new();

  if (get()->_event_base == nullptr) LOG(FATAL) << "libevent base event create failed!";

  auto *listen_ev = event_new(get()->_event_base, listen_fd, EV_READ | EV_PERSIST,
                              accept_handler, (void *)get()->_event_base);

  event_add(listen_ev, nullptr);
  event_base_dispatch(get()->_event_base);  // should be blocked here until exit

  // clean up
  event_del(listen_ev);
  close(listen_fd);
}

void ManagementInterface::schedule_destroy(int fd) {
  std::unique_lock<std::shared_mutex> lock(_mutex);
  _destroy_list.push_back(fd);
}

void ManagementInterface::accept_handler(evutil_socket_t fd, short event, void *arg) {
  struct sockaddr_un sin {};
  socklen_t len = sizeof(sin);

  // waiting for new connection accepted
  int new_fd;
  if ((new_fd = accept(fd, (struct sockaddr *)&sin, (socklen_t *)&len)) < 0) {
    LOG(ERROR) << "Error when handling incoming connection";
    return;
  } else {
    VLOG(3) << "Handle incoming local management connection for fd:[" << fd << "]";
  }

  // dispatch task to new thread
  auto new_container = std::make_unique<ConnectionContainer>(
      new_fd, [new_fd]() { get()->schedule_destroy(new_fd); });

  // clean up old connection (since fd will not be reused)
  auto old_container = get()->_thread_management_container.find(new_fd);
  if (old_container != get()->_thread_management_container.end()) {
    get()->_thread_management_container.erase(old_container);
    VLOG(3) << "clean up old connection for fd:[" << new_fd << "]";
  }

  get()->_thread_management_container.emplace(new_fd, std::move(new_container));

  VLOG(3) << "new thread for fd:[" << new_fd << "] is created from [" << sin.sun_path << "]";
}
ManagementInterface::~ManagementInterface() {
  struct timeval delay = {10, 0};
  event_base_dispatch(_event_base);
  event_base_loopexit(_event_base, &delay);
  event_base_free(get()->_event_base);
  LOG(INFO) << "event base loop exit, with delay: " << delay.tv_sec << "s " << delay.tv_usec
            << "us";

  // clean up old connection
  for (auto &it : _thread_management_container) {
    LOG(INFO) << "clean up old connection for fd:[" << it.first << "]";
    _thread_management_container.erase(it.first);
  }
}
}  // namespace P2PFileSync