#ifndef P2P_FILE_SYNC_INTERFACE_CONNECTION_SESSION_H
#define P2P_FILE_SYNC_INTERFACE_CONNECTION_SESSION_H

#include <any>
#include <filesystem>
#include <string>
#include <memory>
#include <unordered_map>
#include <utility>

#include "utils/status.h"
#include "utils/ip_addr.h"
#include "server_kit/server_kit.h"

namespace P2PFileSync {
class ConnectionSession {
 public:
  ConnectionSession() = delete;
  ConnectionSession(std::shared_ptr<Serverkit::UserContext> session);

  template <typename T>
  T get_option(const char * key);

  void del_option(const std::string& key);

  bool exist_option(const std::string& key);

  void store_option(const std::string& key, std::any value);

  void set_register_status(bool status);

  static bool get_register_status() {
    return P2PFileSync::ConnectionSession::register_status;
  }

  std::shared_ptr<Serverkit::UserContext>& get_usr_ctx() {
    return _user_session;
  }

 private:
  // server management
  inline static bool register_status = false;

  /**
   *  Following variables are genearted per session
   */
  std::shared_ptr<Serverkit::UserContext> _user_session;

  std::unordered_map<std::string, std::any> _session_storage;
};

}  // namespace P2PFileSync
#endif  // P2P_FILE_SYNC_INTERFACE_CONNECTION_SESSION_H