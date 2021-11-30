//
// Created by hanzech on 11/29/21.
//

#include "user_login_response.h"
namespace P2PFileSync::Serverkit {

UserLoginResponse::UserLoginResponse(char* json) : IJsonModel(json){};

std::string UserLoginResponse::user_id() const { return get_value<std::string>("user_id"); }

std::string UserLoginResponse::user_email() const { return get_value<std::string>("email"); }
}  // namespace P2PFileSync::Serverkit