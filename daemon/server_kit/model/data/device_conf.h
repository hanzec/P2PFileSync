#ifndef P2P_FILE_SYNC_Serverkit_MODEL_DATA_DEVICE_CONF_H
#define P2P_FILE_SYNC_Serverkit_MODEL_DATA_DEVICE_CONF_H

#include <filesystem>
#include <string>

#include "../IJsonModel.h"
#include "../response/client/register_client_response.h"
#include "../../../utils/ip_addr.h"

namespace P2PFileSync::ServerKit {
/**
 * @brief Request Model of Client Register
 */
class DeviceConfiguration : public IJsonModel {
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
  [[nodiscard]] std::vector<IPAddr> device_ip() const;


  /**
   * @brief Set the IP address section of RegisterClientRequest
   *
   * @param ipaddress the ip address without port as string
   */
  void set_device_ip(const std::string& jwt_key);

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
}  // namespace P2PFileSync::ServerKit

#endif  // P2P_FILE_SYNC_Serverkit_MODEL_DATA_DEVICE_CONF_H