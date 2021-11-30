//
// Created by hanzech on 11/29/21.
//

#include "user_detail_response.h"

namespace P2PFileSync::Serverkit {

UserDetailResponse::UserDetailResponse(char* json) : IJsonModel(json){};

std::string UserDetailResponse::name() const { return get_value<std::string>("username"); }

std::string UserDetailResponse::email() const { return get_value<std::string>("email"); }

}  // namespace P2PFileSync::Serverkit
