#ifndef P2P_FILE_SYNC_SERVER_KIT_MODEL_REQUEST_REGISTER_CLIENT_RESPONSE_H
#define P2P_FILE_SYNC_SERVER_KIT_MODEL_REQUEST_REGISTER_CLIENT_RESPONSE_H

#include <string>
#include "../IJsonModel.hpp"

namespace P2PFileSync::Server_kit {
/**
 * @brief Construct a new Register Client Request object
 * @note for any giving pointer as argument, this object will take over the
 * management
 * @note for any object as returned value, the caller need to take responsable for free
 */
class RegisterClientResponse : public IJsonModel {
 public:
  /**
   * @brief Construct a new Register Client Response object
   *
   */
  RegisterClientResponse() = default;

  /**
   * @brief Construct a new Register Client Response object from exist json ptr,
   * @note the json ptr will be managed by this object after construct, please
   * DO NOT free or modify this ptr after RegisterClientResponse object
   * construct!
   *
   * @param json the json string wants to parse as Register Client Response Object
   */
  RegisterClientResponse(char* json);

  /**
   * @brief Get the register code section in RegisterClientResponse request
   * 
   * @return int the register code in integer
   */
  int get_register_code();

  /**
   * @brief Get the client id ection in RegisterClientResponse request
   * 
   * @return std::shared_ptr<std::string> the client id registered in server 
   */
  std::string get_client_id();

  /**
   * @brief Get the enable url ection in RegisterClientResponse request
   * 
   * @return std::shared_ptr<std::string> the url for enabling this client
   */
  std::string get_enable_url();


/**
 * @brief Get the raw psck12 certificate section in RegisterClientResponse request
 * 
 * @return std::shared_ptr<std::string> the raw psck12 certificate encoded by BASE64
 */
  std::string get_raw_psck12_certificate();
};
}  // namespace P2PFileSync::Server_kit

#endif  // P2P_FILE_SYNC_SERVER_KIT_MODEL_REQUEST_REGISTER_CLIENT_RESPONSE_H