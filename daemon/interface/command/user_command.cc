//
// Created by hanzech on 11/29/21.
//
#include "../command_executor.h"

namespace P2PFileSync {
class UserCommand : public AutoRegCommand<UserCommand> {
  REGISTER_COMMAND(UserCommand, USER, DEFAULT);

 public:
  void exec(std::ostringstream& out, const std::vector<std::string>& args) final {
    if(args.empty()) {
      out << "Usage: USER [COMMAND] [OPTIONS]";
      return;
    }

    auto& user_ctx = session()->get_usr_ctx();

    // Handle login
    if(args[0] == "LOGIN"){
      if(args.size() < 3) {
        out << "Usage: USER LOGIN <email> <password>";
        return;
      }

      if(user_ctx->is_logged_in()) {
        out << "You are already logged in.";
        return;
      }

      if(user_ctx->login(args[1], args[2])) {
        out << "Login successful.";
      } else {
        out << "Login failed.";
      }
    } else if(args[0] == "INFO") {
      if(args.size() != 1){
        out << "Usage: USER INFO";
        return;
      }

      if(!user_ctx->is_logged_in()) {
        out << "You are not logged in.";
        return;
      }
      auto info = user_ctx->user_detail();
      out << "User info:" << std::endl;
      out << "  name: " << info->name() << std::endl;
      out << "  email: " << info->email() << std::endl;
    } else if (args[0] == "LOGOUT") {
      if(args.size() != 1){
        out << "Usage: USER LOGOUT";
        return;
      }

      if(!user_ctx->is_logged_in()) {
        out << "You are not logged in.";
        return;
      }

      user_ctx->logout();
    } else {
      out << "Unknown command: " << args[0];
    }

  };
};
}  // namespace P2PFileSync
