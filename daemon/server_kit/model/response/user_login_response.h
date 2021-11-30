//
// Created by hanzech on 11/29/21.
//

#ifndef P2P_FILE_SYNC_USER_LOGIN_RESPONSE_H
#define P2P_FILE_SYNC_USER_LOGIN_RESPONSE_H
#include "../IJsonModel.h"

//todo moodify the document
namespace P2PFileSync::Serverkit {

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
  UserLoginResponse(char* json);

  std::string user_id() const;

  std::string user_email() const;
};
}
#endif  // P2P_FILE_SYNC_USER_LOGIN_RESPONSE_H
