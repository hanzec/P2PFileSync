#ifndef P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REGISTER_CLIENT_REQUEST_H
#define P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REGISTER_CLIENT_REQUEST_H

#include <string>

#include "../IJsonModel.hpp"

namespace P2PFileSync::Serverkit {

/**
 * @brief Request Model of Client Register
 * 
 */
class RegisterClientRequest : public IJsonModel {
 public:
/**
  * @brief Construct a new Register Client Request object
  * //TODO changed its constructor with argument instread using setter
  */
  RegisterClientRequest() = default;

  /**
   * @brief Construct a new Register Client Request object from exist json ptr, 
   * @note the json ptr will be managed by this object after construct, please DO NOT 
   *       free or modify this ptr after RegisterClientRequest object construct!
   * 
   * @param json the json string wants to parse as Register Client Request
   */
  RegisterClientRequest(char* json);

  /**
   * @brief Set the Machine ID section of RegisterClientRequest
   * 
   * @param machine_id the machine id as string
   */
  void setMachineID(const std::string& machine_id);

  /**
   * @brief Set the IP address section of RegisterClientRequest 
   * 
   * @param ipaddress the ip address without port as string
   */
  void setIPAddress(const std::string& ipaddress);
};
}  // namespace P2PFileSync::Serverkit

#endif  // P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REGISTER_CLIENT_REQUEST_H