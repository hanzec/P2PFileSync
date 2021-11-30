#ifndef P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REQUEST_CLIENT_CERTIFICATE_RESPONSE_H
#define P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REQUEST_CLIENT_CERTIFICATE_RESPONSE_H

#include <string>

#include "model/IJsonModel.h"

namespace P2PFileSync::Serverkit {
/**
 * @brief Construct a new Register Client Request object
 * @note for any giving pointer as argument, this object will take over the
 * management
 * @note for any object as returned value, the caller need to take responsable
 * for free
 */
class ClientCertResponse : public IJsonModel {
 public:
  /**
   * @brief Construct a new request client certificate object with empty response
   * is deleted
   *
   */
  ClientCertResponse() = delete;

  /**
   * @brief Construct a new request client certificate object from exist json ptr,
   * @note the json ptr will be managed by this object after construct, please
   * DO NOT free or modify this ptr after RequestClientCertResponse object
   * construct!
   *
   * @param json the json string wants to parse as request client certificate
   * Object
   */
  ClientCertResponse(char* json);

  /**
   * @brief Get the client id in RequestClientCertResponse request
   *
   * @return std::string the client id registered in server
   */
  EXPORT_FUNC std::string get_client_id();

  /**
   * @brief Get the PSCK12 certificate encoded in base64 in
   * RequestClientCertResponse request
   *
   * @return std::string the raw jwt login token certificate encoded by
   * BASE64
   */
  EXPORT_FUNC std::string get_client_PSCK12_certificate();
};
}  // namespace P2PFileSync::Serverkit

#endif  // P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REQUEST_CLIENT_CERTIFICATE_RESPONSE_H