//
// Created by hanzech on 11/29/21.
//

#include "user_login_response.h"
namespace P2PFileSync::ManagementAPI {

UserLoginResponse::UserLoginResponse(char* json) : JsonModel(json){};

std::string UserLoginResponse::user_id() const { return get_value<std::string>("user_id").value(); }

std::string UserLoginResponse::user_email() const { return get_value<std::string>("email").value(); }
}  // namespace P2PFileSync::ManagementAPI