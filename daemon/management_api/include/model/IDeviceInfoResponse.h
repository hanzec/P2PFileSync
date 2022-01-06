//
// Created by chen_ on 2022/1/2.
//

#ifndef P2P_FILE_SYNC_IDEVICEINFORESPONSE_H
#define P2P_FILE_SYNC_IDEVICEINFORESPONSE_H
#include <string>
#include "management_api_export.h"
namespace P2PFileSync::ManagementAPI {
/**
 * @brief Construct a new Register Client Request object
 * @note for any giving pointer as argument, this object will take over the
 * management
 * @note for any object as returned value, the caller need to take responsable
 * for free
 */
class MANAGEMENT_API_EXPORT IDeviceInfoResponse {
 public:
  /**
   * @brief Construct a new request device certificate object with empty
   * response is deleted
   *
   */
  MANAGEMENT_API_NO_EXPORT IDeviceInfoResponse() = default;

  MANAGEMENT_API_NO_EXPORT virtual ~IDeviceInfoResponse() = default;

  /**
   * @brief Get the device id in RequestClientInfo Response object
   *
   * @return std::string the device id registered in server
   */
  [[nodiscard]] MANAGEMENT_API_EXPORT virtual std::string device_id() const = 0;

  /**
   * @brief Get the machine id of the response in RequestClientInfo Response object
   *
   * @return std::string the raw jwt login token certificate encoded by
   * BASE64
   */
  [[nodiscard]] MANAGEMENT_API_EXPORT virtual std::string machine_id() const = 0;

  /**
   * @brief Get the device ip address in RequestClientInfo Response object
   *
   * @return std::string the raw jwt login token certificate encoded by
   * BASE64
   */
  [[nodiscard]] MANAGEMENT_API_EXPORT virtual std::string device_ip() const = 0;

  /**
   * @brief Get the the register operator in RequestClientInfo Response object
   *
   * @return std::string the raw jwt login token certificate encoded by
   * BASE64
   */
  [[nodiscard]] MANAGEMENT_API_EXPORT virtual std::string register_user() const = 0;
};
}  // namespace P2PFileSync::ManagementAPI
#endif  // P2P_FILE_SYNC_IDEVICEINFORESPONSE_H
