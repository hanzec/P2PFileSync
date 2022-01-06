#ifndef P2P_FILE_SYNC_INTERFACE_CONNECTION_CONTAINER_H
#define P2P_FILE_SYNC_INTERFACE_CONNECTION_CONTAINER_H

#include <atomic>
#include <functional>
#include <thread>

#include "command_base.h"
#include "utils/command_parser.h"

namespace P2PFileSync {
/**
 * @brief The ConnectionContainer class
 * @note this class is used to store all the information about the connection
 * and the command which is used to process the connection
 */
class ConnectionContainer {
 public:
  /**
   * @brief make sure this object only construct and get by init()
   */
  ConnectionContainer(const ConnectionContainer& obj) = delete;
  ConnectionContainer(const ConnectionContainer&& obj) = delete;
  ConnectionContainer& operator=(const ConnectionContainer& obj) = delete;
  ConnectionContainer& operator=(const ConnectionContainer&& obj) = delete;

  ~ConnectionContainer() noexcept;

  ConnectionContainer(ConnectionContainer&& obj) noexcept;

  explicit ConnectionContainer(
      int socket_fd, const std::function<void()>& destroy_callback = [] {}) noexcept;

 protected:
  /**
   * @brief the thread function of the connection container
   *
   * @param socket_fd the socket fd of the connection
   */
  void run(int socket_fd) noexcept;

  /**
   * @brief execute the giving command
   *
   * @param out the output stream which command output print to
   * @param cmd the command which need to be execute
   * @param args the arguments of the command
   * @return Status return the status code after exec_command
   */
  bool execute_command(std::ostringstream& output, const ParsedCommand& command) noexcept;

 private:
  std::thread _running_thread;
  std::atomic<bool> _running_flag = true;
  std::shared_ptr<ConnectionSession> _session;
  const std::function<void()>& _destroy_callback;
  std::unordered_map<std::string_view, std::unique_ptr<Command>> _instance_map{};
};

}  // namespace P2PFileSync
#endif  // P2P_FILE_SYNC_INTERFACE_CONNECTION_CONTAINER_H