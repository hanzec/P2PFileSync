//
// Created by chen_ on 2022/1/2.
//

#ifndef P2P_FILE_SYNC_USER_DETAIL_RESPONSE_H_EXPORT
#define P2P_FILE_SYNC_USER_DETAIL_RESPONSE_H_EXPORT
#include <unordered_map>
#include "management_api_export.h"

namespace P2PFileSync::ManagementAPI {
class MANAGEMENT_API_EXPORT IUserDetailResponse {
 public:
  /**
   * @brief Construct a new Register Client Response object
   *
   */
  MANAGEMENT_API_NO_EXPORT IUserDetailResponse() = default;

  MANAGEMENT_API_NO_EXPORT virtual ~IUserDetailResponse() = default;

  [[nodiscard]] MANAGEMENT_API_EXPORT virtual std::string name() const = 0;

  [[nodiscard]] MANAGEMENT_API_EXPORT virtual std::string email() const = 0;

  [[nodiscard]] MANAGEMENT_API_EXPORT virtual std::unordered_map<int, std::pair<std::string, std::string>> group()
      const = 0;
};

}  // namespace P2PFileSync::ManagementAPI
#endif  // P2P_FILE_SYNC_USER_DETAIL_RESPONSE_H_EXPORT
