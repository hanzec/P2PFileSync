#include "device_conf.h"

#include <rapidjson/istreamwrapper.h>

#include <fstream>

namespace P2PFileSync::ServerKit {
DeviceConfiguration::DeviceConfiguration(const std::filesystem::path& device_cfg_file)
    : IJsonModel() {
  // load json file from disk
  if (std::filesystem::exists(device_cfg_file)) {
    std::ifstream ifs(device_cfg_file);
    rapidjson::IStreamWrapper isw(ifs);
    get_root().ParseStream(isw);
  } else {
    throw std::runtime_error("Device configuration file not found");
  }
}

DeviceConfiguration::DeviceConfiguration(std::string&& device_id, std::string&& jwt_key) {
  if(device_id.empty() || jwt_key.empty()) {
    throw std::runtime_error("Device id or jwt key is empty");
  }

  add_value("jwt_key", jwt_key);
  add_value("device_id", device_id);
};

DeviceConfiguration::DeviceConfiguration(
    const RegisterClientResponse& register_device_response)
    : DeviceConfiguration(register_device_response.device_id(),
                          register_device_response.jwt_key()) {}

void DeviceConfiguration::set_jwt_key(const std::string& jwt_key) {
  this->add_value("jwt_key", jwt_key);
};

void DeviceConfiguration::set_device_id(const std::string& device_id) {
  this->add_value("device_id", device_id);
}
std::string DeviceConfiguration::jwt_key() const {
  return get_value<std::string>("jwt_key");
}
std::string DeviceConfiguration::device_id() const {
  return get_value<std::string>("device_id");
}
}  // namespace P2PFileSync::ServerKit