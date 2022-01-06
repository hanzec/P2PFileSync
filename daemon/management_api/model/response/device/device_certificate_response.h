#ifndef P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_CLIENT_CLIENT_CERTIFICATE_RESPONSE_H
#define P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_CLIENT_CLIENT_CERTIFICATE_RESPONSE_H
#include <string>
#include "model/json_model.h"
#include "management_api_export.h"

namespace P2PFileSync::ManagementAPI {
/**
 * @brief Construct a new Register Client Request object
 * @note for any giving pointer as argument, this object will take over the
 * management
 * @note for any object as returned value, the caller need to take responsible
 * for free
 */
class ClientCertResponse : public JsonModel {
 public:
  /**
   * @brief Construct a new request device certificate object with empty response
   * is deleted
   *
   */
  ClientCertResponse() = delete;

  /**
   * @brief Construct a new request device certificate object from exist json ptr,
   * @note the json ptr will be managed by this object after construct, please
   * DO NOT free or modify this ptr after RequestClientCertResponse object
   * construct!
   *
   * @param json the json string wants to parse as request device certificate
   * Object
   */
  explicit ClientCertResponse(char* json);

  /**
   * @brief Get the device id in RequestClientCertResponse request
   *
   * @return std::string the device id registered in server
   */
  MANAGEMENT_API_EXPORT std::string device_id();

  /**
   * @brief Get the PSCK12 certificate encoded in base64 in
   * RequestClientCertResponse request
   *
   * @return std::string the raw jwt login token certificate encoded by
   * BASE64
   */
  MANAGEMENT_API_EXPORT std::string PSCK12_certificate();
};
}  // namespace P2PFileSync::ManagementAPI

#endif  // P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_CLIENT_CLIENT_CERTIFICATE_RESPONSE_H