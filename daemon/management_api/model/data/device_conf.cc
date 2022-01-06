#include "device_conf.h"


#include <fstream>

namespace P2PFileSync::ManagementAPI {
DeviceConfiguration::DeviceConfiguration(const std::filesystem::path& device_cfg_file)
    : JsonModel(device_cfg_file) {}

DeviceConfiguration::DeviceConfiguration(
    const RegisterClientResponse& register_device_response)
    : JsonModel() {
  if (register_device_response.device_id().empty() ||
      register_device_response.jwt_key().empty()) {
    throw std::runtime_error("Device id or jwt key is empty");
  }

  add_value("jwt_key", register_device_response.jwt_key());
  add_value("device_id", register_device_response.device_id());
  //  add_array("device_ip",register_device_response.ip_address());
}

std::string DeviceConfiguration::jwt_key() const {
  return get_value<std::string>("jwt_key").value();
}

std::string DeviceConfiguration::device_id() const {
  return get_value<std::string>("device_id").value();
}

std::vector<std::string> DeviceConfiguration::device_ip() const {
  return get_array<std::string>("device_ip");
}

}  // namespace P2PFileSync::ManagementAPI