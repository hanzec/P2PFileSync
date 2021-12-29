//
// Created by hanzech on 11/29/21.
//


#ifndef P2P_FILE_SYNC_USER_DETAIL_RESPONSE_H
#define P2P_FILE_SYNC_USER_DETAIL_RESPONSE_H
#include <unordered_map>
#include "../../IJsonModel.h"

namespace P2PFileSync::ServerKit {

class UserDetailResponse : public IJsonModel {
 public:
  /**
   * @brief Construct a new Register Client Response object
   *
   */
  UserDetailResponse() = delete;

  /**
   * @brief Construct a new Register Client Response object from exist json ptr,
   * @note the json ptr will be managed by this object after construct, please
   * DO NOT free or modify this ptr after RegisterClientResponse object
   * construct!
   *
   * @param json the json string wants to parse as Register Client Response
   * Object
   */
  explicit UserDetailResponse(char* json);

  [[nodiscard]] std::string name() const;

  [[nodiscard]] std::string email() const;

  [[nodiscard]] std::unordered_map<int,std::pair<std::string, std::string>> group() const;

};

}  // namespace P2PFileSync::ServerKit
#endif  // P2P_FILE_SYNC_USER_DETAIL_RESPONSE_H
