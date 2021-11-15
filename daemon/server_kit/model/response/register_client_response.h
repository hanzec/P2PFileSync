#ifndef P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REGISTER_CLIENT_RESPONSE_H
#define P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REGISTER_CLIENT_RESPONSE_H

#include <string>

#include "../IJsonModel.h"

namespace P2PFileSync::Serverkit {
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
  RegisterClientResponse(char* json);

  /**
   * @brief Get the register code section in RegisterClientResponse request
   *
   * @return int the register code in integer
   */
  EXPORT_FUNC int get_register_code();

  /**
   * @brief Get the client id ection in RegisterClientResponse request
   *
   * @return std::string the client id registered in server
   */
  EXPORT_FUNC std::string get_client_id();

  /**
   * @brief Get the enable url ection in RegisterClientResponse request
   *
   * @return std::string the url for enabling this client
   */
  EXPORT_FUNC std::string get_enable_url();

  /**
   * @brief Get the jwt login token section in RegisterClientResponse
   * request
   *
   * @return std::string the raw jwt login token certificate encoded by
   * BASE64
   */
  EXPORT_FUNC std::string get_login_token();
};
}  // namespace P2PFileSync::Serverkit

#endif  // P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REGISTER_CLIENT_RESPONSE_H