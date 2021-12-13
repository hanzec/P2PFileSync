//
// Created by hanzech on 11/30/21.
//

#include "user_group_response.h"

#include "model/response/public_method.h"
P2PFileSync::ServerKit::UserGroupResponse::UserGroupResponse(char *json) : IJsonModel(json) {}
std::unordered_map<int, std::pair<std::string, std::string>>

P2PFileSync::ServerKit::UserGroupResponse::group() const {
  return get_user_group_from_node(get_raw_node("user_group"));
}
