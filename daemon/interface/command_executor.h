/*
 * @Author: Hanze CHen
 * @Date: 2021-08-30 19:34:05
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-10-24 17:03:28
 */
#ifndef P2P_FILE_SYNC_MANAGE_INTERFACE_COMMAND_EXECTORS_H
#define P2P_FILE_SYNC_MANAGE_INTERFACE_COMMAND_EXECTORS_H

#include <glog/logging.h>

#include <algorithm>
#include <functional>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "connection_session.h"
#include "utils/log.h"

namespace P2PFileSync {
using COMMAND_ARG = const std::vector<std::string>;

/**
 * @brief The base class for the command
 * @note for every command, the program will create instance of the
 * the command object for all different incoming file descriptor by
 * creating multiple instance of CommandExecutor for each created connection, and
 * destroyed when connection is close or have exceptions
 */
class CommandBase {
 public:
  /**
   * @brief Construct a new Command Base object
   *
   * @param daemon_status
   */
  explicit CommandBase(std::shared_ptr<ConnectionSession> daemon_status) {
    _daemon_status = std::move(daemon_status);
  };

  /**
   * @brief the function which perform the command function
   *
   * @param output the output stream of the command
   * @param arguments the argument parsed from command parser as vector
   */
  virtual void exec(std::ostringstream& output, COMMAND_ARG& arguments) = 0;

 protected:
  std::shared_ptr<ConnectionSession>& session() { return _daemon_status; }

  // Prevent anyone from directly inheriting from CommandBase
  virtual void DoNotInheritFromThisClassButAutoRegCommandInstead() = 0;

 private:
  std::shared_ptr<ConnectionSession> _daemon_status;
};

using PTRC_OBJECT = std::function<CommandBase*(std::shared_ptr<ConnectionSession>)>;

class CommandExecutor {
 public:
  CommandExecutor();

  /**
   * @brief Destroy the CommandExecutor object, when destroy this object will do
   * following thins:
   *  1. Clean all created existed command interface in _instance_map
   */
  ~CommandExecutor();

  /**
   * @brief pprint the help message
   *
   * @param output the outout stream which help message will print out
   */
  static void get_help_msg(std::ostringstream& output) noexcept;

  /**
   * @brief execute the giving command
   *
   * @param out the output stream which command output print to
   * @param cmd the command which need to be execute
   * @param args the arguments of the command
   * @return Status return the status code after exec_command
   */
  void exec(std::ostringstream& out, const std::string& cmd, COMMAND_ARG& args);

  /**
   * @brief Helper class for expose the private method outside
   *
   */
  class RegisterHelper {
   protected:
    static bool reg_command_wrapper(const std::string_view& cmd, const std::string_view& desc,
                                   PTRC_OBJECT&& obj) noexcept {
      return reg_command(std::forward<PTRC_OBJECT>(obj), cmd, desc);
    };
  };

 private:
  // TODO command requirement need to be implemented
  /**
   * @brief Connection session for every CommandExecutor object
   */
  std::shared_ptr<ConnectionSession> _session;

  /**
   * @brief Per connection fd handler for command instance
   */
  std::unordered_map<std::string_view, CommandBase*> _instance_map{};

  /**
   * @brief static map storage command handler construct function and its
   * description by command str as key
   */
  inline static std::unordered_map<std::string_view, std::pair<std::string_view, PTRC_OBJECT>>
      _constructor_map;

  /**
   * @brief private function which regist command dynamically
   *
   * @param command the command in string
   * @param description the short description of the string
   * @param command_handler the pointer of command handler function
   * @return bool the registation status of current regist process
   */
  static bool reg_command(PTRC_OBJECT&& command_obj, const std::string_view& command,
                          const std::string_view& description);
};

/**
 * @brief Auto register helper template for Command
 *
 * @tparam T the command need to mark as auto regiter
 */
template <typename T>
class AutoRegCommand : public CommandBase, private CommandExecutor::RegisterHelper {
 public:
  explicit AutoRegCommand(std::shared_ptr<ConnectionSession> session) : CommandBase(session) {
    (void)AutoRegCommand<T>::_reg_status;
  };

  static const constexpr std::string_view COMMAND_NAME;

  static const constexpr std::string_view COMMAND_DESCRIPTION;

 protected:
  // Prevent anyone from directly inheriting from CommandBase
  void DoNotInheritFromThisClassButAutoRegCommandInstead() final{};

 private:
  /**
   * registation status which shared accross all command class
   */
  static const bool _reg_status;
};

template <typename T>
const bool AutoRegCommand<T>::_reg_status = AutoRegCommand<T>::reg_command_wrapper(
    T::COMMAND_NAME, T::COMMAND_DESCRIPTION, [](auto a) { return new T(a); });
}  // namespace P2PFileSync

#define REGISTER_COMMAND(class, name, description)                    \
 public:                                                              \
  explicit class(std::shared_ptr<ConnectionSession> session)          \
      : AutoRegCommand<class>(std::move(session)){};                  \
  static const constexpr std::string_view COMMAND_NAME = "" #name ""; \
  static const constexpr std::string_view COMMAND_DESCRIPTION = "" #description "";

#endif  // P2P_FILE_SYNC_MANAGE_INTERFACE_COMMAND_EXECTORS_H
