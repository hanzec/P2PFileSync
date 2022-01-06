//
// Created by hanzech on 12/18/21.
//

#ifndef P2P_FILE_SYNC_COMMAND_FACTORY_H
#define P2P_FILE_SYNC_COMMAND_FACTORY_H
#include <functional>

#include "command_base.h"
#include "connection_session.h"

namespace P2PFileSync {

using PTRC_OBJECT =
    std::function<std::unique_ptr<Command>(std::shared_ptr<ConnectionSession>&)>;

class CommandFactory {
 public:
  /**
   * Delete the default constructor and copy constructor to prevent the class to be copied.
   */
  CommandFactory() = delete;
  ~CommandFactory() = delete;
  CommandFactory(CommandFactory&) = delete;
  CommandFactory(CommandFactory&&) = delete;
  CommandFactory& operator=(CommandFactory&) = delete;
  CommandFactory& operator=(CommandFactory&&) = delete;

  /**
   * @brief pprint the help message
   *
   * @param output the outout stream which help message will print out
   */
  static void get_help_msg(std::ostringstream& output) noexcept;

  /**
   * @brief register a command
   *
   * @param command_name the name of the command
   * @param command_ptr the pointer to the command
   */
  static std::unique_ptr<Command> get_command_obj(
      const std::string& command_name, std::shared_ptr<ConnectionSession>& daemon_status);

  /**
   * @brief Helper class for expose the private method outside
   *
   * @param cmd the name of the command
   * @param desc the description of the command
   * @param obj the reference of std::function for creating the command object
   */
  class RegisterHelper {
   protected:
    static bool reg_command(const std::string_view& cmd, const std::string_view& desc,
                            PTRC_OBJECT&& obj) noexcept {
      return reg_command_internal(std::forward<PTRC_OBJECT>(obj), cmd, desc);
    };
  };

 private:
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
   * @param command_obj the reference of std::function for creating the command object
   * @return bool the registration status of current register process
   */
  static bool reg_command_internal(PTRC_OBJECT&& command_obj, const std::string_view& command,
                                   const std::string_view& description);
};

/**
 * @brief Auto register helper template for Command
 *
 * @tparam T the command need to mark as auto regiter
 */
template <typename T>
class AutoRegCommand : public Command, private CommandFactory::RegisterHelper {
 public:
  explicit AutoRegCommand(std::shared_ptr<ConnectionSession> session) : Command(session) {
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
const bool AutoRegCommand<T>::_reg_status = AutoRegCommand<T>::reg_command(
    T::COMMAND_NAME, T::COMMAND_DESCRIPTION, [](auto a) { return std::make_unique<T>(a); });
}  // namespace P2PFileSync

#define REGISTER_COMMAND(class, name, description)                    \
 public:                                                              \
  explicit class(std::shared_ptr<ConnectionSession> session)          \
      : AutoRegCommand<class>(std::move(session)){};                  \
  static const constexpr std::string_view COMMAND_NAME = "" #name ""; \
  static const constexpr std::string_view COMMAND_DESCRIPTION = "" #description "";
#endif  // P2P_FILE_SYNC_COMMAND_FACTORY_H
