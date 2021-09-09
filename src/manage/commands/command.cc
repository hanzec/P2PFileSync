#include "command.h"
#include <unordered_map>

#include "utils/log.h"
#include "utils/status.h"

namespace P2PFileSync {
// def of class static member
std::unordered_map<std::string,
                   std::pair<std::string, COMMAND_HANDLER>> CommandFactory::_handler_map;

// print help message
void CommandFactory::get_help_msg(std::ostringstream& output) noexcept {
  for (const auto& pair : CommandFactory::_handler_map) {
    output << std::setw(15) << pair.first << std::setw(0) << pair.second.first
           << std::endl;
  }
};

// command factory producer
Status CommandFactory::exec_command(std::ostringstream& output,
                                    const std::string& command,
                                    const std::vector<std::string>& arguments) {
  // the only special case will handle by command factory
  if (command == "help") {
    CommandFactory::get_help_msg(output);
    return Status::OK();
  }

  auto handler = CommandFactory::_handler_map.find(command);

  // if command not found
  if (handler != CommandFactory::_handler_map.end()) {
    output << "command[" << command << "] not found!" << std::endl;
    return {StatusCode::UNAVAILABLE, "command not found"};
  }

  // execute the command
  handler->second.second(output,arguments);

  return {StatusCode::OK, "command execute successfully"};
}

// register avaliable command
Status CommandFactory::register_object(const std::string& command,
                                       const std::string& description,
                                       COMMAND_HANDLER command_handler) {
  auto handler = CommandFactory::_handler_map.find(command);
  if (handler != CommandFactory::_handler_map.end()) {
    LOG(ERROR) << "Command [" << command << "] Already Registered !";
    return {StatusCode::INVALID_ARGUMENT, "Command name already registered!"};
  } else {
    VLOG(VERBOSE) << "Command name [" << command << "] registered !";
  }
  _handler_map.emplace(command, std::make_pair(description, std::move(command_handler)));
  return Status::OK();
}

}  // namespace P2PFileSync
