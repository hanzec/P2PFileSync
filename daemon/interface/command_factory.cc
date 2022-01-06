#include "command_factory.h"
#include <glog/logging.h>
namespace P2PFileSync {
// print help message
void CommandFactory::get_help_msg(std::ostringstream& output) noexcept {
  for (const auto& pair : CommandFactory::_constructor_map) {
    output << std::setw(15) << pair.first << std::setw(0) << pair.second.first << std::endl;
  }
};

// register available command
bool CommandFactory::reg_command_internal(PTRC_OBJECT&& command_obj,
                                          const std::string_view& command,
                                          const std::string_view& description) {
  if (CommandFactory::_constructor_map.find(command) !=
      CommandFactory::_constructor_map.end()) {
    return false;
  }
  CommandFactory::_constructor_map.emplace(
      command, std::make_pair(description, std::move(command_obj)));
  return true;
}

std::unique_ptr<Command> CommandFactory::get_command_obj(
    const std::string& command_name, std::shared_ptr<ConnectionSession>& daemon_status) {
  auto it = CommandFactory::_constructor_map.find(command_name);
  if (it == CommandFactory::_constructor_map.end()) {
    LOG(WARNING) << "command " << command_name << " not found";
    return nullptr;
  }
  return it->second.second(daemon_status);
}

}  // namespace P2PFileSync
