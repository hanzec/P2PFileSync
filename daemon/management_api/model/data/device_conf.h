#ifndef P2P_FILE_SYNC_Serverkit_MODEL_DATA_DEVICE_CONF_H
#define P2P_FILE_SYNC_Serverkit_MODEL_DATA_DEVICE_CONF_H

#include <string>
#include <filesystem>

#include "model/json_model.h"
#include "model/response/device/register_client_response.h"

namespace P2PFileSync::ManagementAPI {
/**
 * @brief Request Model of Client Register
 */
class DeviceConfiguration : public JsonModel {
 public:
   /**
   * @brief Construct a new Device configuration file
   */
  explicit DeviceConfiguration(const std::filesystem::path& device_cfg_file);

  /**
   * @brief Construct a new Device configuration file
   */
  explicit DeviceConfiguration(const RegisterClientResponse& register_device_response);

  /**
   * @brief Set the IP address section of RegisterClientRequest
   *
   * @param ipaddress the ip address without port as string
   */
  [[nodiscard]] std::string jwt_key() const;

  /**
   * @brief Set the Machine ID section of RegisterClientRequest
   *
   * @param machine_id the machine id as string
   */
  [[nodiscard]] std::string device_id() const;


  /**
   * @brief Set the Machine ID section of RegisterClientRequest
   *
   * @param machine_id the machine id as string
   */
  [[nodiscard]] std::vector<std::string> device_ip() const;
};
}  // namespace P2PFileSync::ManagementAPI

#endif  // P2P_FILE_SYNC_Serverkit_MODEL_DATA_DEVICE_CONF_H