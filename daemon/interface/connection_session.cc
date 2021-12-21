//
// Created by hanzech on 12/18/21.
//

#include "connection_session.h"
namespace P2PFileSync {
template <typename T>
T ConnectionSession::get_option(const char* key) {
  return std::any_cast<T>(_session_storage[key]);
}

template <>
int ConnectionSession::get_option(const char* key) {
  return std::any_cast<int>(_session_storage[key]);
}
void ConnectionSession::del_option(const std::string& key) { _session_storage.erase(key); }

bool ConnectionSession::exist_option(const std::string& key) {
  auto it{_session_storage.find(key)};
  return it != std::end(_session_storage);
}

void ConnectionSession::store_option(const std::string& key, std::any value) {
  _session_storage[key] = std::move(value);
}

}  // namespace P2PFileSync