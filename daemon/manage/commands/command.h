/*
 * @Author: Hanze CHen
 * @Date: 2021-08-30 19:34:05
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-09-09 14:24:57
 */
#ifndef P2P_FILE_SYNC_MANAGE_COMMAND_COMMAND_H
#define P2P_FILE_SYNC_MANAGE_COMMAND_COMMAND_H

#include <glog/logging.h>

#include <algorithm>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../../daemon_state.h"
#include "../../utils/log.h"
#include "../../utils/status.h"

namespace P2PFileSync {
using COMMAND_HANDLER = std::function<void(std::ostringstream&, DaemonStatus&,
                                           const std::vector<std::string>&)>;

class CommandFactory {
 public:
  /**
   * @brief pprint the help message
   *
   * @param output the outout stream which help message will print out
   */
  static void get_help_msg(std::ostringstream& output) noexcept;

  /**
   * @brief execute the giving command
   *
   * @param output the output stream which command output print to
   * @param command the command which need to be execute
   * @param arguments the arguments of the command
   * @return Status return the status code after exec_command
   */
  [[nodiscard]] static Status exec_command(
      std::ostringstream& output, const std::string& command,
      DaemonStatus& daemon_status, const std::vector<std::string>& arguments);

  /**
   * @brief Helper class for expose the private method outside
   *
   */
  class CommandBase {
   public:
    static Status register_object_warper(const std::string& command,
                                         const std::string& description,
                                         COMMAND_HANDLER command_handler) {
      return register_object(command, description, std::move(command_handler));
    };
  };

 private:
  /**
   * @brief static map storage command handler ptr and its description by
   * command str as key
   *
   */
  static std::unordered_map<std::string,
                            std::pair<std::string, COMMAND_HANDLER>>
      _handler_map;

  /**
   * @brief private function which regist command dynamically
   *
   * @param command the command in string
   * @param description the short description of the string
   * @param command_handler the pointer of command handler function
   * @return Status the registation status of current regist process
   */
  static Status register_object(const std::string& command,
                                const std::string& description,
                                COMMAND_HANDLER command_handler);
};
}  // namespace P2PFileSync

/**
 * @brief Macros for generating auto register command handler
 *
 */
#define NEW_COMMAND(command, description, output, args, daemon_status) \
  namespace P2PFileSync::AUTO_GEN_COMMAND {                            \
  class command : private P2PFileSync::CommandFactory::CommandBase {   \
   public:                                                             \
    command() { (void)_reg_status; };                                  \
    static void do_operation(std::ostringstream& output,               \
                             DaemonStatus& daemon_status,              \
                             const std::vector<std::string>& args);    \
                                                                       \
   private:                                                            \
    const P2PFileSync::Status _reg_status = register_object_warper(    \
        "" #command "", description, command::do_operation);           \
  };                                                                   \
  };                                                                   \
  void P2PFileSync::AUTO_GEN_COMMAND::command::do_operation(           \
      std::ostringstream& output, DaemonStatus& daemon_status,         \
      const std::vector<std::string>& args)

#endif
