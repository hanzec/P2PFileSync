
#include "connection_session.h"

namespace P2PFileSync {

ConnectionSession::ConnectionSession() : management_(P2PFileSync_SK_new_session(false)){
  if(!get_register_status()){
    set_register_status(P2PFileSync_SK_is_registered());
  }
};

template <typename T>
T ConnectionSession::get_option(const char* key) {
  return std::any_cast<T>(session_storage[key]);
}

template <>
int ConnectionSession::get_option(const char* key) {
  return std::any_cast<int>(session_storage[key]);
}
void ConnectionSession::del_option(const std::string& key) {
  session_storage.erase(key);
}

bool ConnectionSession::exist_option(const std::string& key) {
  auto it{session_storage.find(key)};
  return it != std::end(session_storage);
}

void ConnectionSession::store_option(const std::string& key, std::any value) {
  session_storage[key] = std::move(value);
}

void ConnectionSession::set_register_status(bool status) {
  P2PFileSync::ConnectionSession::register_status = status;
}



}  // namespace P2PFileSync
