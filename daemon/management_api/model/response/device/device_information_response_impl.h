#ifndef P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_CLIENT_CLIENT_INFORMATION_RESPONSE_IMPL_H
#define P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_CLIENT_CLIENT_INFORMATION_RESPONSE_IMPL_H
#include <string>
#include "model/json_model.h"
#include "model/IDeviceInfoResponse.h"

namespace P2PFileSync::ManagementAPI {
/**
 * @brief Construct a new Register Client Request object
 * @note for any giving pointer as argument, this object will take over the
 * management
 * @note for any object as returned value, the caller need to take responsable
 * for free
 */
class DeviceInfoResponseImpl : public JsonModel, public IDeviceInfoResponse {
 public:
  /**
   * @brief Construct a new request device certificate object with empty
   * response is deleted
   *
   */
  DeviceInfoResponseImpl() = delete;

  ~DeviceInfoResponseImpl() override= default;

  /**
   * @brief Construct a new request device certificate object from exist json
   * ptr,
   * @note the json ptr will be managed by this object after construct, please
   * DO NOT free or modify this ptr after RequestClientInfoResponse object
   * construct!
   *
   * @param json the json string wants to parse as request device certificate
   * Object
   */
  explicit DeviceInfoResponseImpl(char* json);

  /**
   * @brief Get the device id in RequestClientInfo Response object
   *
   * @return std::string the device id registered in server
   */
  std::string device_id() const override;

  /**
   * @brief Get the machine id of the response in RequestClientInfo Response object
   *
   * @return std::string the raw jwt login token certificate encoded by
   * BASE64
   */
  std::string machine_id() const override;

  /**
   * @brief Get the device ip address in RequestClientInfo Response object
   *
   * @return std::string the raw jwt login token certificate encoded by
   * BASE64
   */
  std::string device_ip() const override;

  /**
   * @brief Get the the register operator in RequestClientInfo Response object
   *
   * @return std::string the raw jwt login token certificate encoded by
   * BASE64
   */
  std::string register_user() const override;
};
}  // namespace P2PFileSync::ManagementAPI

#endif  // P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_CLIENT_CLIENT_INFORMATION_RESPONSE_IMPL_H