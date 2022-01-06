//
// Created by hanzech on 11/27/21.
//
#include "../command_factory.h"

namespace P2PFileSync {
class InfoCommand : public AutoRegCommand<InfoCommand> {
  REGISTER_COMMAND(InfoCommand, INFO, DEFAULT);
 public:
  void exec(std::ostringstream& out, const std::vector<std::string>& args) final {
    if (!args.empty()) {
      out << "INFO command does not need any arguments!";
      return;
    }

    auto device_ctx = ManagementAPI::dev_ctx();
    auto device_info = device_ctx->device_info();

    out << "Device info: " << std::endl;
    out << "  Device id: " << device_info->device_id() << std::endl;
    out << "  Device status: " << std::boolalpha << device_ctx->is_enabled() << std::endl;
    out << "  Device hardware id: " << device_info->machine_id() << std::endl;
    out << "  Device Registered ip: " << device_info->device_ip() << std::endl;
    out << "  Device Register User: " << device_info->register_user();
  };
};
}  // namespace P2PFileSync