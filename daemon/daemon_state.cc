
#include "daemon_state.h"

namespace P2PFileSync {

DaemonStatus::DaemonStatus() : management_(P2PFileSync_SK_new_session(false)){};

template <typename T>
T DaemonStatus::get_option(const std::string& key) {
  return std::any_cast<T>(session_storage[key]);
}

void DaemonStatus::del_option(const std::string& key) {
  session_storage.erase(key);
}

bool DaemonStatus::exist_option(const std::string& key) {
  auto it{session_storage.find(key)};
  return it != std::end(session_storage);
}

void DaemonStatus::store_option(const std::string& key, std::any value) {
  session_storage[key] = std::move(value);
}

void DaemonStatus::set_register_status(bool status) {
  P2PFileSync::DaemonStatus::register_status = status;
}



}  // namespace P2PFileSync
