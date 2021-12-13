
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

#include "interface/command_executor.h"
#include "utils/log.h"
#include "utils/parsing.h"

namespace P2PFileSync {
::std::shared_ptr<ManagementInterface> ManagementInterface::init(const std::string &socket,
                                                                 int listen_type) {
  if (_instance == nullptr) {
    return _instance = create(socket, listen_type);
  } else {
    return _instance;
  }
}

ManagementInterface::ManagementInterface(const ManagementInterface::this_is_private &,
                                         const std::string &socket_addr, int listen_type,
                                         int timeout)
    : _timeout(timeout) {
  VLOG(INFO) << "starting manage thread listen on [" << socket_addr << "]";

  if (socket_addr.length() > 108) {
    LOG(FATAL) << "socket file path cannot be longer than 108 character";
  }

  // set up file descriptor
  if ((_listen_socket = socket(listen_type, SOCK_STREAM, 0)) == -1) {
    LOG(FATAL) << "can not create socket: " << strerror(errno);
  } else {
    VLOG(3) << "create socket success, socket fd: " << _listen_socket;
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
    IPAddr ip_addr(socket_addr);
    actual_size = sizeof(struct sockaddr_in);
    auto *addr_in = (struct sockaddr_in *)&addr;
    addr_in->sin_family = listen_type;
    addr_in->sin_port = htons(ip_addr.port());
    inet_aton(ip_addr.ip().c_str(), &addr_in->sin_addr);
    VLOG(3) << "socket is AF_INET and ip is: " << ip_addr.ip()
            << " and port is: " << ip_addr.port();
  } else {
    LOG(FATAL) << "unsupported listen type";
  }

  // bind socket between socket and file descriptor
  if (bind(_listen_socket, (struct sockaddr *)&addr, actual_size) == -1) {
    LOG(FATAL) << "cannot bind open socket to [" << socket_addr << "]: " << strerror(errno);
  } else {
    VLOG(3) << "bind socket to [" << socket_addr << "] success";
  }

  // connect
  _manage_interface_thread = std::thread(&ManagementInterface::manage_interface_thread, this);
}
void ManagementInterface::manage_interface_thread() {
  // connect
  if (listen(_listen_socket, 10) < 0) { /* tell kernel we're a server */
    LOG(FATAL) << "cannot set socket [" << _listen_socket << "] status to listen";
  }

  // libevent
  auto *base = event_base_new();

  if (base == nullptr) LOG(FATAL) << "libevent base event create failed!";

  auto *listen_ev =
      event_new(base, _listen_socket, EV_READ | EV_PERSIST, accept_handler, (void *)base);

  event_add(listen_ev, nullptr);
  event_base_dispatch(base);
  event_del(listen_ev);
  event_base_free(base);
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
  get()->_executor_container_running_flag[new_fd] = true;
  get()->_thread_management_container.emplace(
      new_fd, std::move(std::thread(&ManagementInterface::executor_container, new_fd,
                                    get()->_timeout)));

  VLOG(3) << "new thread for fd:[" << new_fd << "] is created";
}
void ManagementInterface::executor_container(int new_fd, int timeout) {
  CommandExecutor ce;
  std::ostringstream command_buf;
  auto &flag = get()->_executor_container_running_flag[new_fd];

  // setup timeout
#if defined(__linux__)
  struct timeval tv {};
  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  if (setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    LOG(ERROR) << "setsockopt failed";
  }
#else
  DWORD timeout = timeout_in_seconds * 1000;
  if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout) <
      0) {
    LOG(ERROR) << "setsockopt failed";
  }
#endif

  // main loop
  while (flag) {
    write(new_fd, ">",1);

    // reading command from socket
    char buf;
    size_t nread;
    while ((nread = read(new_fd, &buf, 1)) > 0) {
      command_buf << buf;
      if (buf == '\n') break;
    }

    // error detection
    if (nread == 0) {
      LOG(INFO) << "connection closed by peer";
      break;
    } else if (nread == -1) {
      LOG(ERROR) << "Error when reading from socket";
      break;
    }

    if(!command_buf.str().empty()){
      // parsing command
      COMMAND command;
      auto parse_ret = P2PFileSync::parsing_command(command_buf.str().c_str(), command);

      // check if parsing is failed
      if (!parse_ret.ok()) {
        auto err_msg = parse_ret.message();
        write(new_fd, err_msg.c_str(), err_msg.length());
        VLOG(3) << "parsing command failed";
      } else {
        std::ostringstream os;
        ce.exec(os, command.first, command.second);
        write(new_fd, os.str().c_str(), os.str().length());
        VLOG(3) << "executing command " << command.first << " success";
      }
    }
    command_buf.str("");
    command_buf.clear();
  }

  // close socket
  close(new_fd);

  // remove thread from container
  // todo need to clean up thread
  //  get()->_thread_management_container.erase(new_fd);
}

}  // namespace P2PFileSync