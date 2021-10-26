#include <climits>
#include <memory>
#include <utility>

#include "../command_executor.h"
#include "server_kit/server_kit.h"

namespace P2PFileSync {
class RegisterCommand : public AutoRegCommand<RegisterCommand> {
 public:
  RegisterCommand(std::shared_ptr<ConnectionSession> session)
      : AutoRegCommand<RegisterCommand>(std::move(session)){};

  static const constexpr std::string_view COMMAND_NAME = "REGISTER";

  static const constexpr std::string_view COMMAND_DESCRIPTION = "REGISTER";

  void exec(std::ostringstream& out,
            const std::vector<std::string>& args) final {
    auto ds = get_demon_status();
    if (!ds->get_register_status()) {
      if (args.size() == 1) {
        if (_register_pin != INT_MAX) {
          if (std::stoi(args[0]) == _register_pin) {
            ds->set_register_status(true);
            out << "client successful registered";
          }
        } else {
          out << "Error: you should run REGISTER without argument first";
        }
      } else if (args.size() == 0) {
        auto ret = P2PFileSync_SK_register_client();

        if (!P2PFileSync_SK_success(ret)) {
          out << "failed to register new client to management server!";
        } else {
          _register_pin = *static_cast<int*>(P2PFileSync_SK_get_data(ret, 0));
          out << "regist request sent, please visit ["
              << static_cast<char*>(P2PFileSync_SK_get_data(ret, 1)) << "]";
        }
      } else {
        out << "Error: [REGISTER] will only take at most 1 arguments";
      }
    } else {
      out << "client is already registered!";
    }
  };

 private:
  int _register_pin = INT_MAX;
};
}  // namespace P2PFileSync