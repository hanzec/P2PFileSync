//
// Created by hanzech on 11/29/21.
//

#ifndef P2P_FILE_SYNC_USER_LOGIN_RESPONSE_H
#define P2P_FILE_SYNC_USER_LOGIN_RESPONSE_H
#include "model/IJsonModel.h"

//todo modify the document
namespace P2PFileSync::ServerKit {

class UserLoginResponse :public IJsonModel {
 public:
  /**
   * @brief Construct a new Register Client Response object
   *
   */
  UserLoginResponse() = delete;

  /**
   * @brief Construct a new Register Client Response object from exist json ptr,
   * @note the json ptr will be managed by this object after construct, please
   * DO NOT free or modify this ptr after RegisterClientResponse object
   * construct!
   *
   * @param json the json string wants to parse as Register Client Response
   * Object
   */
  explicit UserLoginResponse(char* json);

  [[nodiscard]] std::string user_id() const;

  [[nodiscard]] std::string user_email() const;
};
}
#endif  // P2P_FILE_SYNC_USER_LOGIN_RESPONSE_H
