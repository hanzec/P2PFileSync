#include "command_executor.h"

namespace P2PFileSync {
// print help message
void CommandExecutor::get_help_msg(std::ostringstream& output) noexcept {
  for (const auto& pair : CommandExecutor::_constructor_map) {
    output << std::setw(15) << pair.first << std::setw(0) << pair.second.first << std::endl;
  }
};

// command factory producer
void CommandExecutor::exec(std::ostringstream& output, const std::string& command,
                           COMMAND_ARG& arguments) {
  // the only special case will handle_difficult by command factory
  if (command == "help") {
    CommandExecutor::get_help_msg(output);
  } else {
    // print debug info
    if (VLOG_IS_ON(3)) {
      std::ostringstream oss;

      if (!arguments.empty()) {
        // Convert all but the last element to avoid a trailing ","
        std::copy(arguments.begin(), arguments.end() - 1,
                  std::ostream_iterator<std::string>(oss, ","));

        // Now add the last element with no delimiter
        oss << arguments.back();
      }
      VLOG(3) << "command: " << command << " arguments: [" << oss.str() << "]";
    }

    // return failied when command not found
    auto command_index = CommandExecutor::_constructor_map.find(command);
    if (command_index == CommandExecutor::_constructor_map.end()) {
      output << "command [" << command << "] not found!" << std::endl;
      return;
    }

    // looking for the command_obj
    std::unique_ptr<CommandBase> command_obj{nullptr};
    auto command_obj_index = _instance_map.find(command);
    if (command_obj_index == _instance_map.end()) {
      // construct command object
      command_obj = std::move(command_index->second.second(
          std::forward<std::shared_ptr<ConnectionSession>>(_session)));

      // execute the obj
      command_obj->exec(output, arguments);

      // add to managed interface map
      CommandExecutor::_instance_map.emplace(command, std::move(command_obj));
    } else {
      command_obj_index->second->exec(output, arguments);
    }

    // add new line;
    output << std::endl;
  }
}

// register avaliable command
bool CommandExecutor::reg_command(PTRC_OBJECT&& command_obj, const std::string_view& command,
                                  const std::string_view& description) {
  if (CommandExecutor::_constructor_map.find(command) !=
      CommandExecutor::_constructor_map.end()) {
    return false;
  }
  CommandExecutor::_constructor_map.emplace(
      command, std::make_pair(description, std::move(command_obj)));
  return true;
}
CommandExecutor::CommandExecutor():_session(std::make_shared<ConnectionSession>(ServerKit::ServerContext::get_usr_ctx())) {}

}  // namespace P2PFileSync
