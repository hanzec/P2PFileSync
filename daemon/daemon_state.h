#ifndef P2P_FILE_SYNC_DAEMON_STATUS_H
#define P2P_FILE_SYNC_DAEMON_STATUS_H

#include <any>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <utility>

#include "common.h"
#include "server_kit/server_kit.h"
#include "utils/ip_addr.h"
#include "utils/status.h"

namespace P2PFileSync {
class DaemonStatus {
 public:
  DaemonStatus(P2P_SYNC_SERVER_SESSION* management) : management_(management){};

  template <typename T>
  T get_option(const std::string& key) {
    return std::any_cast<T>(session_storage[key]);
  }

  void del_option(const std::string& key) { session_storage.erase(key); }

  bool exist_option(const std::string& key) {
    auto it{session_storage.find(key)};
    return it != std::end(session_storage);
  }

  void store_option(const std::string& key, std::any value) {
    session_storage[key] = std::move(value);
  }

  static void set_register_status(bool status) {
    P2PFileSync::DaemonStatus::register_status = status;
  }

  static bool get_register_status() {
    return P2PFileSync::DaemonStatus::register_status;
  }

 private:
  // Per-session variables;
  P2P_SYNC_SERVER_SESSION* management_;
  std::unordered_map<std::string, std::any> session_storage;
  // server management
  static bool register_status;
};

}  // namespace P2PFileSync
#endif  // P2P_FILE_SYNC_DAEMON_STATUS_H