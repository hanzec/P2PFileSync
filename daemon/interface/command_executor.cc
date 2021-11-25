#include "command_executor.h"

namespace P2PFileSync {
CommandExecuter::~CommandExecuter() {
  for (const auto& pair : _instance_map) {
    free(pair.second);
    VLOG(3) << "destroy [" << pair.first << "] object";
  }
};

// print help message
void CommandExecuter::get_help_msg(std::ostringstream& output) noexcept {
  for (const auto& pair : CommandExecuter::_constructor_map) {
    output << std::setw(15) << pair.first << std::setw(0) << pair.second.first
           << std::endl;
  }
};

// command factory producer
void CommandExecuter::exec(std::ostringstream& output,
                           const std::string& command, COMMAND_ARG& arguments) {
  // the only special case will handle_difficult by command factory
  if (command == "help") {
    CommandExecuter::get_help_msg(output);
  } else {
    CommandBase* command_obj;

    // return failied when command not found
    auto command_index = CommandExecuter::_constructor_map.find(command);
    if (command_index == CommandExecuter::_constructor_map.end()) {
      output << "command [" << command << "] not found!" << std::endl;
      return;
    }

    // looking for the command_obj
    auto command_obj_index = _instance_map.find(command);
    if (command_obj_index == _instance_map.end()) {
      // construct command object
      command_obj = command_index->second.second(
          std::forward<std::shared_ptr<ConnectionSession>>(_session));

      // add to managed interface map
      CommandExecuter::_instance_map.emplace(command, command_obj);
    } else {
      command_obj = command_obj_index->second;
    }

    // execute the obj
    command_obj->exec(output, arguments);

    // add new line;
    output << std::endl;
  }
}

// register avaliable command
bool CommandExecuter::reg_command(PTRC_OBJECT&& command_obj,
                                  const std::string_view& command,
                                  const std::string_view& description) {
  if (CommandExecuter::_constructor_map.find(command) !=
      CommandExecuter::_constructor_map.end()) {
    return false;
  }
  CommandExecuter::_constructor_map.emplace(
      command, std::make_pair(description, std::move(command_obj)));
  return true;
}

}  // namespace P2PFileSync
