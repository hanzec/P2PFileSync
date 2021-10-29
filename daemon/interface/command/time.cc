#include <chrono>
#include <ctime>
#include <utility>

#include "../command_executor.h"

namespace P2PFileSync {
class TimeCommand : public AutoRegCommand<TimeCommand> {
  REGISTER_COMMAND(TimeCommand, TIME, DEFAULT);

 public:
  void exec(std::ostringstream& out,
            const std::vector<std::string>& args) final {
    if (args.size() > 0) {
      out << "TIME command does not need any arguments!";
    }

    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    out << std::ctime(&end_time);
  };
};
}  // namespace P2PFileSync