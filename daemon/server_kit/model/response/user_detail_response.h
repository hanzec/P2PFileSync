//
// Created by hanzech on 11/29/21.
//


#ifndef P2P_FILE_SYNC_USER_DETAIL_RESPONSE_H
#define P2P_FILE_SYNC_USER_DETAIL_RESPONSE_H
#include "../IJsonModel.h"

namespace P2PFileSync::Serverkit {

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
  UserDetailResponse(char* json);

  std::string name() const;

  std::string email() const;
};

}  // namespace P2PFileSync::Serverkit
#endif  // P2P_FILE_SYNC_USER_DETAIL_RESPONSE_H
