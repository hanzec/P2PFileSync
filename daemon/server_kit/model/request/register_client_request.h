#ifndef P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REGISTER_CLIENT_REQUEST_H
#define P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REGISTER_CLIENT_REQUEST_H

#include <string>

#include "../IJsonModel.h"

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
  RegisterClientRequest();

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

  /**
   * @brief Set the Group Name of current client belongs
   * @note by default this would be set as "DEFAULT_GROUP" if this method is not
   * called. "DEFAULT_GROUP" is an auto-generated default group in server side
   * @param groupname
   */
  void setGroupName(const std::string& groupname);
};
}  // namespace P2PFileSync::Serverkit

#endif  // P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_REGISTER_CLIENT_REQUEST_H