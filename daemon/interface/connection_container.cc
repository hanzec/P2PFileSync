#include "connection_container.h"

#include "command_factory.h"
#include "environment.h"

namespace P2PFileSync {

ConnectionContainer::ConnectionContainer(
    int socket_fd, const std::function<void()>& destroy_callback) noexcept
    : _session(std::make_shared<ConnectionSession>()),
      _destroy_callback(destroy_callback),
      _running_thread(std::move(std::thread(&ConnectionContainer::run, this, socket_fd))) {
  VLOG(2) << "ConnectionContainer created for socket " << socket_fd;
};

ConnectionContainer::~ConnectionContainer() noexcept {
  VLOG(3) << "ConnectionContainer destroyed";
  _running_flag = false;
  if (_running_thread.joinable()) {
    VLOG(2) << "Stop signal send, Waiting for container to finish...";
    _running_thread.join();
  }
}

ConnectionContainer::ConnectionContainer(ConnectionContainer&& obj) noexcept
    : _session(std::move(obj._session)),
      _destroy_callback(obj._destroy_callback),
      _running_thread(std::move(obj._running_thread)) {
  VLOG(2) << "ConnectionContainer moved from " << &obj;
}

void ConnectionContainer::run(int socket_fd) noexcept {
  std::ostringstream command_buf;

  // setup timeout
#if defined(UNIX) or defined(MACOSX)
  struct timeval tv {};
  // todo modify here avoid hard code
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    LOG(ERROR) << "SocketRocket failed";
  }
#else
  DWORD timeout = timeout_in_seconds * 1000;
  if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout) < 0) {
    LOG(ERROR) << "setsockopt failed";
  }
#endif

  // main loop
  while (_running_flag.load()) {
    write(socket_fd, kTerminalMessage.cbegin(), kTerminalMessage.size());

    // reading command from socket
    char buf='\0';
    size_t read_size = 0;
    while ((read_size = read(socket_fd, &buf, 1)) > 0) {
      if(read_size == 1){
        command_buf << buf;
        if (buf == '\n') break;
      }
    }

    if (!command_buf.str().empty()) {
      // parsing command
      ParsedCommand command;
      auto parse_ret = P2PFileSync::parse_command(command_buf.str(), command);

      // check if parsing is failed
      if (!parse_ret.ok()) {
        auto err_msg = parse_ret.message();
        write(socket_fd, err_msg.c_str(), err_msg.length());
        VLOG(3) << "parsing command failed";
      } else {
        std::ostringstream os;
        if(execute_command(os, command)) {
          VLOG(3) << "executing command " << command.first << " success";
        } else {
          VLOG(1) << "executing command " << command.first << " failed";
        }
        write(socket_fd, os.str().c_str(), os.str().length());
      }
    }
    command_buf.str("");
    command_buf.clear();
  }

  // close socket
  close(socket_fd);

  // call destroy callback
  _destroy_callback();
}

bool ConnectionContainer::execute_command(std::ostringstream& output,
                                          const ParsedCommand& command) noexcept {
  // the only special case will handle_difficult by command factory
  if (command.first == "help") {
    CommandFactory::get_help_msg(output);
    return true;
  } else {
    // debug info
    if (VLOG_IS_ON(3)) {
      std::ostringstream debug_string;

      if (!command.second.empty()) {
        // Convert all but the last element to avoid a trailing ","
        std::copy(command.second.begin(), command.second.end() - 1,
                  std::ostream_iterator<std::string>(debug_string, ","));

        // Now add the last element with no delimiter
        debug_string << command.second.back();
      }
      VLOG(3) << "command: " << command.first << " arguments: [" << debug_string.str() << "]";
    }

    // execute command
    auto command_ptr = _instance_map.find(command.first);
    if (command_ptr != _instance_map.end()) {
      command_ptr->second->exec(output, command.second);
    } else {
      auto cmd_obj = CommandFactory::get_command_obj(command.first, _session);

      if (cmd_obj == nullptr) {
        output << "Command not found" << std::endl << "available commands: ";
        CommandFactory::get_help_msg(output);
        return false;
      } else {
        cmd_obj->exec(output, command.second);
        _instance_map.emplace(command.first, std::move(cmd_obj));
      }
    }
    // add new line;
    output << std::endl;
    return true;
  }
}
}  // namespace P2PFileSync
