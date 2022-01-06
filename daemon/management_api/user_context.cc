
#include <model/response/user/user_login_response.h>

#include "server_endpoint.h"
#include "management_api_impl.h"
#include "utils/curl_utils.h"
#include "model/response/user/user_detail_response_impl.h"

namespace fs = std::filesystem;

namespace P2PFileSync::ManagementAPI {

UserContextImpl::UserContextImpl(const std::string& server_address)
    : _share_handle(curl_share_init()), _server_address(server_address) {
  curl_share_setopt(_share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
  curl_share_setopt(_share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
}

UserContextImpl::~UserContextImpl() { curl_share_cleanup(_share_handle); };

void UserContextImpl::logout() {
  if (_login) {
    _login = false;

    // logout do not need any response
    Curl::POST_and_save_to_ptr(_share_handle,
                         std::string(_server_address).append(SERVER_PASSWORD_LOGOUT_V1),
                         nullptr, true, false);

    VLOG(2) << "logout for current user session";
  }
}

std::pair<bool, std::string> UserContextImpl::login(const std::string& email,
                                                const std::string& password) {
  if (_login) return {true, "user already login"};  // avoid re-login

  UserLoginResponse response(static_cast<char*>(Curl::POST_and_save_to_ptr(
      _share_handle, std::string(_server_address).append(SERVER_PASSWORD_LOGIN_V1),
      ("email=" + email + "&password=" + password).c_str(), false, false)));

  if (response.success() && response.user_email() == email) {
    _login = true;
    return {true, "login success"};
  } else {
    return {false, response.message()};
  }
}

std::unique_ptr<IUserDetailResponse> UserContextImpl::user_detail() const {
  if (!_login) return nullptr;

  auto response = std::make_unique<UserDetailResponseImpl>(static_cast<char*>(Curl::GET_and_save_to_ptr(
      _share_handle, std::string(_server_address).append(SERVER_USER_DETAIL_V1), {}, false)));

  if (response->success()) {
    return response;
  } else {
    return nullptr;
  }
}

bool UserContextImpl::is_logged_in() const { return _login; }

}  // namespace P2PFileSync::ManagementAPI