//
// Created by hanzech on 11/30/21.
//
#ifndef P2P_FILE_SYNC_USER_GROUP_RESPONSE_H
#define P2P_FILE_SYNC_USER_GROUP_RESPONSE_H
#include "model/json_model.h"
#include <unordered_map>

namespace P2PFileSync::ManagementAPI {
class UserGroupResponse: public JsonModel {
  /**
   * @brief Construct a new Register Client Response object
   *
   */
  UserGroupResponse() = delete;

  /**
   * @brief Construct a new Register Client Response object from exist json ptr,
   * @note the json ptr will be managed by this object after construct, please
   * DO NOT free or modify this ptr after RegisterClientResponse object
   * construct!
   *
   * @param json the json string wants to parse as Register Client Response
   * Object
   */
  explicit UserGroupResponse(char* json);

  [[nodiscard]] std::unordered_map<int,std::pair<std::string, std::string>> group() const;

};
}
#endif  // P2P_FILE_SYNC_USER_GROUP_RESPONSE_H
