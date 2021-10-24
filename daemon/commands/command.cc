#include "command.h"

#include <unordered_map>

namespace P2PFileSync {
// print help message
void CommandFactory::get_help_msg(std::ostringstream& output) noexcept {
  for (const auto& pair : CommandFactory::_handler_map) {
    output << std::setw(15) << pair.first << std::setw(0) << pair.second.first
           << std::endl;
  }
};

// command factory producer
bool CommandFactory::exec_command(std::ostringstream& output,
                                  const std::string& command,
                                  DaemonStatus& daemon_status,
                                  const std::vector<std::string>& arguments) {
  // the only special case will handle by command factory
  if (command == "help") {
    CommandFactory::get_help_msg(output);
    return true;
  }

  auto handler = CommandFactory::_handler_map.find(command);

  // if command not found
  if (handler != CommandFactory::_handler_map.end()) {
    output << "command[" << command << "] not found!" << std::endl;
    return false;
  }

  // execute the command
  handler->second.second(output, daemon_status, arguments);

  return true;
}

// register avaliable command
bool CommandFactory::register_object(const std::string& command,
                                     const std::string& description,
                                     COMMAND_HANDLER command_handler) {
  auto handler = CommandFactory::_handler_map.find(command);
  if (handler != CommandFactory::_handler_map.end()) {
    LOG(ERROR) << "Command [" << command << "] Already Registered !";
    return false;
  } else {
    VLOG(VERBOSE) << "Command name [" << command << "] registered !";
  }
  CommandFactory::_handler_map.emplace(command,std::make_pair(description, std::move(command_handler)));
  return true;
}

}  // namespace P2PFileSync
