#ifndef P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REGISTER_CLIENT_RESPONSE_H
#define P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REGISTER_CLIENT_RESPONSE_H

#include <string>

#include "../../IJsonModel.h"

namespace P2PFileSync::ServerKit {
/**
 * @brief Construct a new Register Client Request object
 * @note for any giving pointer as argument, this object will take over the
 * management
 * @note for any object as returned value, the caller need to take responsable
 * for free
 */
class RegisterClientResponse : public IJsonModel {
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
  EXPORT_FUNC int get_register_code();

  /**
   * @brief Get the client id ection in RegisterClientResponse request
   *
   * @return std::array<std::byte,16> the client id registered in server
   */
  [[nodiscard]] EXPORT_FUNC std::string device_id() const;

  /**
   * @brief Get the enable url ection in RegisterClientResponse request
   *
   * @return std::string the url for enabling this client
   */
  [[nodiscard]] EXPORT_FUNC std::string get_enable_url() const;


  [[nodiscard]] EXPORT_FUNC std::vector<std::string> ip_address() const;

  /**
   * @brief Get the jwt login token section in RegisterClientResponse
   * request
   *
   * @return std::string the raw jwt login token certificate encoded by
   * BASE64
   */
  [[nodiscard]] EXPORT_FUNC std::string jwt_key() const;
};
}  // namespace P2PFileSync::ServerKit

#endif  // P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REGISTER_CLIENT_RESPONSE_H