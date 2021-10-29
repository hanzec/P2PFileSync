#include <climits>
#include <cstddef>
#include <memory>
#include <utility>

#include "../command_executor.h"

namespace P2PFileSync {
class RegisterCommand : public AutoRegCommand<RegisterCommand> {
  REGISTER_COMMAND(RegisterCommand, REGISTER, DEFAULT);
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
        auto ret = Serverkit::regist_client();

        if (ret == nullptr) {
          out << "failed to register new client to management server!";
        } else {
          _register_pin = ret->get_register_code();
          out << "regist request sent, please visit ["  << ret->get_enable_url() << "]";
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