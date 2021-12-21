//
// Created by hanzech on 12/18/21.
//

#ifndef P2P_FILE_SYNC_CONNECTION_SESSION_H
#define P2P_FILE_SYNC_CONNECTION_SESSION_H
#include <any>
#include <string>

#include "server_kit/server_kit.h"

namespace P2PFileSync {
class ConnectionSession {
 public:
  template <typename T>
  T get_option(const char* key);

  void del_option(const std::string& key);

  bool exist_option(const std::string& key);

  void store_option(const std::string& key, std::any value);

  ServerKit::UserContext& usr_ctx() { return _user_session; }

 private:
  ServerKit::UserContext _user_session = ServerKit::ServerContext::get_usr_ctx();
  std::unordered_map<std::string, std::any> _session_storage;
};
}  // namespace P2PFileSync
#endif  // P2P_FILE_SYNC_CONNECTION_SESSION_H
