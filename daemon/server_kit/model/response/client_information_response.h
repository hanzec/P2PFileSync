#ifndef P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REQUEST_CLIENT_INFORMATION_RESPONSE_H
#define P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REQUEST_CLIENT_INFORMATION_RESPONSE_H

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
class ClientInfoResponse : public IJsonModel {
 public:
  /**
   * @brief Construct a new request client certificate object with empty
   * response is deleted
   *
   */
  ClientInfoResponse() = delete;

  /**
   * @brief Construct a new request client certificate object from exist json
   * ptr,
   * @note the json ptr will be managed by this object after construct, please
   * DO NOT free or modify this ptr after RequestClientInfoResponse object
   * construct!
   *
   * @param json the json string wants to parse as request client certificate
   * Object
   */
  ClientInfoResponse(char* json);

  /**
   * @brief Get the client id in RequestClientInfo Response object
   *
   * @return std::string the client id registered in server
   */
  EXPORT_FUNC std::string get_client_id();

  /**
   * @brief Get the machine id of the response in RequestClientInfo Response object
   *
   * @return std::string the raw jwt login token certificate encoded by
   * BASE64
   */
  EXPORT_FUNC std::string get_client_machine_id();

  /**
   * @brief Get the client ip address in RequestClientInfo Response object
   *
   * @return std::string the raw jwt login token certificate encoded by
   * BASE64
   */
  EXPORT_FUNC std::string get_client_ip_address();

  /**
   * @brief Get the the register operator in RequestClientInfo Response object
   *
   * @return std::string the raw jwt login token certificate encoded by
   * BASE64
   */
  EXPORT_FUNC std::string get_client_register_by();
};
}  // namespace P2PFileSync::Serverkit

#endif  // P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REQUEST_CLIENT_INFORMATION_RESPONSE_H