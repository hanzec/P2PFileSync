#ifndef P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_REGISTER_CLIENT_RESPONSE_H
#define P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_REGISTER_CLIENT_RESPONSE_H

#include <string>

#include "model/json_model.h"
#include "management_api_export.h"

namespace P2PFileSync::ManagementAPI {
/**
 * @brief Construct a new Register Client Request object
 * @note for any giving pointer as argument, this object will take over the
 * management
 * @note for any object as returned value, the caller need to take responsable
 * for free
 */
class RegisterClientResponse : public JsonModel {
 public:
  /**
   * @brief Construct a new Register Client Response object
   *
   */
  RegisterClientResponse() = delete;

  /**
   * @brief Construct a new Register Client Response object from exist json ptr,
   * @note the json ptr will be managed by this object after construct, please
   * DO NOT free or modify this ptr after RegisterClientResponse object
   * construct!
   *
   * @param json the json string wants to parse as Register Client Response
   * Object
   */
  explicit RegisterClientResponse(char* json);

  /**
   * @brief Get the register code section in RegisterClientResponse request
   *
   * @return int the register code in integer
   */
  MANAGEMENT_API_EXPORT int get_register_code();

  /**
   * @brief Get the device id ection in RegisterClientResponse request
   *
   * @return std::array<std::byte,16> the device id registered in server
   */
  [[nodiscard]] MANAGEMENT_API_EXPORT std::string device_id() const;

  /**
   * @brief Get the enable url ection in RegisterClientResponse request
   *
   * @return std::string the url for enabling this device
   */
  [[nodiscard]] MANAGEMENT_API_EXPORT std::string get_enable_url() const;


  [[nodiscard]] MANAGEMENT_API_EXPORT std::vector<std::string> ip_address() const;

  /**
   * @brief Get the jwt login token section in RegisterClientResponse
   * request
   *
   * @return std::string the raw jwt login token certificate encoded by
   * BASE64
   */
  [[nodiscard]] MANAGEMENT_API_EXPORT std::string jwt_key() const;
};
}  // namespace P2PFileSync::ManagementAPI

#endif  // P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_REGISTER_CLIENT_RESPONSE_H