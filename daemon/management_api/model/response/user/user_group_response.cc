//
// Created by hanzech on 11/30/21.
//

#include "user_group_response.h"

#include "model/response/public_method.h"
P2PFileSync::ManagementAPI::UserGroupResponse::UserGroupResponse(char *json) : JsonModel(json) {}
std::unordered_map<int, std::pair<std::string, std::string>>

P2PFileSync::ManagementAPI::UserGroupResponse::group() const {
  return {};
}
