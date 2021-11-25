#ifndef P2P_FILE_SYNC_Serverkit_MODEL_DATA_DEVICE_CONF_H
#define P2P_FILE_SYNC_Serverkit_MODEL_DATA_DEVICE_CONF_H

#include <filesystem>
#include <string>

#include "../IJsonModel.h"
#include "model/response/register_client_response.h"

namespace P2PFileSync::Serverkit {
/**
 * @brief Request Model of Client Register
 */
class DeviceConfiguration : public IJsonModel {
 public:

   /**
   * @brief Construct a new Device configuration file
   */
  DeviceConfiguration(const std::filesystem::path& device_cfg_file);

  /**
   * @brief Construct a new Device configuration file
   */
  DeviceConfiguration(std::string& device_id, std::string& jwt_key);

  /**
   * @brief Construct a new Device configuration file
   */
  DeviceConfiguration(const RegisterClientResponse& register_device_response);

 /**
   * @brief Set the IP address section of RegisterClientRequest
   *
   * @param ipaddress the ip address without port as string
   */
  std::string get_jwt_key();

  /**
   * @brief Set the Machine ID section of RegisterClientRequest
   *
   * @param machine_id the machine id as string
   */
  std::array<std::byte,16> get_device_id();


  /**
   * @brief Set the IP address section of RegisterClientRequest
   *
   * @param ipaddress the ip address without port as string
   */
  void set_jwt_key(const std::string& jwt_key);

  /**
   * @brief Set the Machine ID section of RegisterClientRequest
   *
   * @param machine_id the machine id as string
   */
  void set_device_id(const std::string& device_id);
};
}  // namespace P2PFileSync::Serverkit

#endif  // P2P_FILE_SYNC_Serverkit_MODEL_DATA_DEVICE_CONF_H