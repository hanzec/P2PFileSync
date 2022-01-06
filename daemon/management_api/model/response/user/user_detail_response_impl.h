//
// Created by hanzech on 11/29/21.
//


#ifndef P2P_FILE_SYNC_USER_DETAIL_RESPONSE_IMPL_H
#define P2P_FILE_SYNC_USER_DETAIL_RESPONSE_IMPL_H
#include <unordered_map>
#include "model/json_model.h"
#include "model/IUserDetailResponse.h"

namespace P2PFileSync::ManagementAPI {

class UserDetailResponseImpl : public JsonModel, public IUserDetailResponse {
 public:
  /**
   * @brief Construct a new Register Client Response object
   *
   */
  UserDetailResponseImpl() = delete;

  ~UserDetailResponseImpl() override = default;

  /**
   * @brief Construct a new Register Client Response object from exist json ptr,
   * @note the json ptr will be managed by this object after construct, please
   * DO NOT free or modify this ptr after RegisterClientResponse object
   * construct!
   *
   * @param json the json string wants to parse as Register Client Response
   * Object
   */
  explicit UserDetailResponseImpl(char* json);

  [[nodiscard]] std::string name() const override;

  [[nodiscard]] std::string email() const override;

  [[nodiscard]] std::unordered_map<int,std::pair<std::string, std::string>> group() const override;

};

}  // namespace P2PFileSync::ManagementAPI
#endif  // P2P_FILE_SYNC_USER_DETAIL_RESPONSE_IMPL_H
