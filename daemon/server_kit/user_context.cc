
#include <model/response/user/user_login_response.h>

#include "server_endpoint.h"
#include "server_kit.h"
#include "utils/curl_utils.h"

namespace fs = std::filesystem;

namespace P2PFileSync::ServerKit {

UserContext::UserContext(const this_is_private&, std::string& server_address)
    : _server_address(server_address), _share_handle(curl_share_init()) {
  curl_share_setopt(_share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
  curl_share_setopt(_share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
}

UserContext::~UserContext() { curl_share_cleanup(_share_handle); };

void UserContext::logout() {
  if (_login) {
    _login = false;

    // logout do not need any response
    POST_and_save_to_ptr(_share_handle,
                         std::string(_server_address).append(SERVER_PASSWORD_LOGOUT_V1),
                         nullptr, true, false);

    LOG(INFO) << "logout for user " << _username;
  }
}

bool UserContext::login(const std::string& email, const std::string& password) {
  if (_login) return true;  // avoid re-login

  auto ret = POST_and_save_to_ptr(
      _share_handle, std::string(_server_address).append(SERVER_PASSWORD_LOGIN_V1),
      ("email=" + email + "&password=" + password).c_str(), false,false);

  if (strlen(static_cast<char *>(ret)) == 0) {
    LOG(ERROR) << "empty response";
    return false;
  }

  try {
    UserLoginResponse response(static_cast<char*>(ret));
    if (response.success() && response.user_email() == email) {
      _login = true;
      LOG(INFO) << "login success for user [" << email << "]";
      return true;
    }
  } catch (...) {
    return false;
  }

  return false;
}

std::unique_ptr<UserDetailResponse> UserContext::user_detail() const {
  if (!_login) return nullptr;

  auto ret = GET_and_save_to_ptr(
      _share_handle, std::string(_server_address).append(SERVER_USER_DETAIL_V1), {}, false);

  if (strlen(static_cast<char *>(ret)) == 0) {
    LOG(ERROR) << "empty response";
    return nullptr;
  }

  try {
    return std::make_unique<UserDetailResponse>(static_cast<char*>(ret));
  } catch (...) {
    return nullptr;
  }
}

bool UserContext::is_logged_in() const { return _login; }

}  // namespace P2PFileSync::ServerKit