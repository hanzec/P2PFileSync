//
// Created by hanzech on 11/29/21.
//

#include "user_detail_response.h"
#include "model/response/public_method.h"

namespace P2PFileSync::ServerKit {
// todo do style checking
UserDetailResponse::UserDetailResponse(char* json) : IJsonModel(json){};

std::string UserDetailResponse::name() const { return get_value<std::string>("username"); }

std::string UserDetailResponse::email() const { return get_value<std::string>("email"); }

std::unordered_map<int, std::pair<std::string, std::string>> UserDetailResponse::group()
    const {
  return get_user_group_from_node(get_raw_node("user_group"));
}

}  // namespace P2PFileSync::ServerKit
