
#include "manage.h"

#include <thread>
#include <event.h>
#include <glog/logging.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <cstring>

#include "commands/command.h"
#include "../utils/log.h"
#include "utils/parsing.h"

using namespace P2PFileSync;

inline std::string read_by_line(int fd) {
  std::ostringstream output_buf;

  char char_buf;
  while (read(fd, &char_buf, 1) > 0) {
    output_buf << char_buf;
    if (char_buf == '\0' || char_buf == EOF) {
      break;
    }
  }
  return output_buf.str();
};

void connection_hander(int fd) {
  // print header info
  write(fd, "P2PFileSync\n>", 13);

  while (true) {
    // recv new incoming command
    auto raw_command = read_by_line(fd);

    // parsing command
    COMMAND command;
    auto parse_ret = P2PFileSync::parsing_command(raw_command, command);

    // check if parsing is failed
    if (!parse_ret.ok()) {
      auto err_msg = parse_ret.Message();
      write(fd, err_msg.c_str(), err_msg.length());
    } else {
      // execute the command from factory
      if (command.first == "EXIT") {
        break;
      } else {
        std::ostringstream output_buf;
        auto exec_ret = CommandFactory::exec_command(output_buf,command.first,command.second);

        if (exec_ret.ok()) {
          write(fd, output_buf.str().c_str(), output_buf.str().length());
        } else {
          auto err_msg = exec_ret.Message();
          write(fd, err_msg.c_str(), err_msg.length());
        }
      }
    }
  }
};

void accept_handler(int sock, short event, void* arg) {
  struct sockaddr_in remote_addr;
  int sin_size = sizeof(struct sockaddr_in);
  int new_fd = accept(sock, (struct sockaddr*)&remote_addr, (socklen_t*)&sin_size);
  if (new_fd < 0) {
    LOG(ERROR) << "Error when handling incoming connection";
    return;
  }

  // using std thread will have better compatibility
  std::thread new_command_handler(&connection_hander, new_fd);
}

int P2PFileSync_start_manage_thread(const char* sockets,
                                    listen_type listen_type) {
  VLOG(INFO) << "starting manage thread listen on [" << sockets << "]";

  // set up sockaddr_un structure
  // note: path has to be less than 108 characters
  auto socket_file_str = std::string(sockets);

  if (socket_file_str.length() > 108) {
    LOG(ERROR) << "socket file path cannot be longer than 108 character";
    return -1;
  }

  // init structure
  struct sockaddr_un saddr;
  saddr.sun_family = listen_type;
  memset(saddr.sun_path, '\0', 108);
  memcpy(saddr.sun_path, socket_file_str.c_str(), socket_file_str.length());

  // set up file descripter
  int fd = 0;
  if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    LOG(ERROR) << "can not create socket";
    return -1;
  }

  // bind socket between socket and file discriptor
  if (bind(fd, (struct sockaddr*)&saddr, sizeof(saddr)) == -1) {
    LOG(ERROR) << "cannot bind open socket to [" << sockets << "]";
    return -1;
  }

  // libevent
  struct event_base* base = event_base_new();
  struct event listen_ev;

  event_set(&listen_ev, fd, EV_READ | EV_PERSIST, accept_handler, NULL);
  event_base_set(base, &listen_ev);
  event_add(&listen_ev, NULL);
  event_base_dispatch(base);

  event_del(&listen_ev);
  event_base_free(base);
  return 1;
}