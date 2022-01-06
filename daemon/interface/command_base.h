//
// Created by hanzech on 12/18/21.
//

#ifndef P2P_FILE_SYNC_COMMAND_BASE_H
#define P2P_FILE_SYNC_COMMAND_BASE_H
#include <memory>
#include <vector>

#include "connection_session.h"

namespace P2PFileSync {
/**
 * @brief The base class for the command
 * @note for every command, the program will create instance of the
 * the command object for all different incoming file descriptor by
 * creating multiple instance of CommandFactory for each created connection, and
 * destroyed when connection is close or have exceptions
 */
class Command {
 public:
  virtual ~Command() = default;  // fix compiler warning

  /**
   * @brief Construct a new Command Base object
   *
   * @param daemon_status
   */
  explicit Command(std::shared_ptr<ConnectionSession>& connectionSession)
      : _session_ref(connectionSession){};

  /**
   * @brief the function which perform the command function
   *
   * @param output the output stream of the command
   * @param arguments the argument parsed from command parser as vector
   */
  virtual void exec(std::ostringstream& output, const std::vector<std::string>& arguments) = 0;

 protected:
  std::shared_ptr<ConnectionSession>& session() { return _session_ref; }

  // Prevent anyone from directly inheriting from Command
  virtual void DoNotInheritFromThisClassButAutoRegCommandInstead() = 0;

 private:
  std::shared_ptr<ConnectionSession>& _session_ref;
};
}  // namespace P2PFileSync
#endif  // P2P_FILE_SYNC_COMMAND_BASE_H
