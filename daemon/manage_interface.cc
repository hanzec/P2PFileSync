
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

std::unordered_map<int, std::shared_ptr<CommandExecuter>> executor_map;

inline std::string read_by_line(struct bufferevent *bev) {
  std::ostringstream output_buf;

  char char_buf;
  while (bufferevent_read(bev, &char_buf, 1) > 0) {
    if (char_buf == '\0' || char_buf == EOF) {
      break;
    }
    output_buf << char_buf;
  }
  return output_buf.str();
};

void read_handler(struct bufferevent *bev, void *ce_ptr) {
  std::shared_ptr<CommandExecuter> ce;

  // looking fot exist connamd executor object
  {
    auto fd = bufferevent_getfd(bev);
    auto ce_index = executor_map.find(fd);
    if (ce_index == executor_map.end()) {
      DLOG(INFO) << "create new command executor object for fd " << fd;
      executor_map.emplace(fd, (ce = std::make_shared<CommandExecuter>()));
    } else {
      ce = ce_index->second;
    }
  }

  // recv new incoming command
  auto raw_command = read_by_line(bev);

  // parsing command
  COMMAND command;
  auto parse_ret = P2PFileSync::parsing_command(raw_command, command);

  // check if parsing is failed
  if (!parse_ret.ok()) {
    auto err_msg = parse_ret.Message();
    bufferevent_write(bev, err_msg.c_str(), err_msg.length());
  } else {
    std::ostringstream os;
    ce->exec(os, command.first, command.second);
    bufferevent_write(bev, os.str().c_str(), os.str().size());
  }
};

void error_handler(struct bufferevent *bev, short event, void *arg) {
  executor_map.erase(bufferevent_getfd(bev));
}

void accept_handler(evutil_socket_t fd, short event, void *arg) {
  struct sockaddr_un sin;
  socklen_t slen = sizeof(sin);
  auto *base = (struct event_base *)arg;

  // waiting for new connection accepted
  int new_fd;
  if ((new_fd = accept(fd, (struct sockaddr *)&sin, (socklen_t *)&slen)) < 0) {
    LOG(ERROR) << "Error when handling incoming connection";
    return;
  }else{
    VLOG(3) << "Handle incoming connection for fd:[" << fd << "]";
  }

  auto *bev = bufferevent_socket_new(base, new_fd, BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb(bev, read_handler, nullptr, error_handler, nullptr);
  bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
}

// TODO supoort AF_INET type connection
void manage_interface_thread(const std::string &sockets, int listen_type) {
  VLOG(INFO) << "starting manage thread listen on [" << sockets << "]";

  if (sockets.length() > 108) {
    LOG(FATAL) << "socket file path cannot be longer than 108 character";
  }

  // set up file descripter
  int listen_fd = 0;
  if ((listen_fd = socket(listen_type, SOCK_STREAM, 0)) == -1) {
    LOG(FATAL) << "can not create socket";
  }

  // init structure
  unlink(sockets.c_str());
  struct sockaddr_un addr;
  addr.sun_family = listen_type;
  memcpy(addr.sun_path, sockets.c_str(), sockets.length());

  // bind socket between socket and file discriptor
  if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    LOG(FATAL) << "cannot bind open socket to [" << sockets << "]";
  }

  // connect
  if (listen(listen_fd, 10) < 0) { /* tell kernel we're a server */
    LOG(FATAL) << "cannot bind set [" << sockets << "] as connect";
  }

  // libevent
  auto *base = event_base_new();

  if (base == nullptr) LOG(FATAL) << "libevent base event create failed!";

  auto *listen_ev = event_new(base, listen_fd, EV_READ | EV_PERSIST,
                              accept_handler, (void *)base);

  event_add(listen_ev, nullptr);
  event_base_dispatch(base);
  event_del(listen_ev);
  event_base_free(base);
}
}  // namespace P2PFileSync