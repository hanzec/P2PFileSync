
#include "connection_session.h"

namespace P2PFileSync {

ConnectionSession::ConnectionSession(std::shared_ptr<Serverkit::UserContext> session)
    : _user_session(std::move(session)) {
  if (!get_register_status()) {
    // set_register_status(Serverkit::get_register_status());
  }
};

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

void ConnectionSession::set_register_status(bool status) {
  P2PFileSync::ConnectionSession::register_status = status;
}

}  // namespace P2PFileSync
