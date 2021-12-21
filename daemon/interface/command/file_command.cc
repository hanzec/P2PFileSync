//
// Created by hanzech on 11/27/21.
//
#include "../command_factory.h"

namespace P2PFileSync {
class FileCommand : public AutoRegCommand<FileCommand> {
  REGISTER_COMMAND(FileCommand, FILE, DEFAULT);

 public:
  void exec(std::ostringstream& out, const std::vector<std::string>& args) final {
    auto user_ctx = session()->usr_ctx();

    // check arguments
    if (args.empty()) {
      out << "Usage: file <command> [args]\n";
    }

    // check if the user is logged in
    if(!user_ctx.is_logged_in()){
      out << "You are not logged in.\n";
      return;
    }

    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    out << std::ctime(&end_time);
  };
};
}  // namespace P2PFileSync