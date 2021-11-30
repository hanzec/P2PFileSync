//
// Created by hanzech on 11/27/21.
//
#include "../command_executor.h"

namespace P2PFileSync {
class FileCommand : public AutoRegCommand<FileCommand> {
  REGISTER_COMMAND(FileCommand, TIME, DEFAULT);

 public:
  void exec(std::ostringstream& out, const std::vector<std::string>& args) final {
    if (!args.empty()) {
      out << "TIME command does not need any arguments!";
    }

    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    out << std::ctime(&end_time);
  };
};
}  // namespace P2PFileSync