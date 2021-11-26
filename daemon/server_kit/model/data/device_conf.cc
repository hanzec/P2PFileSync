#include "device_conf.h"


namespace P2PFileSync::Serverkit {
void DeviceConfiguration::set_jwt_key(const std::string& jwt_key) {
  this->add_value("jwt_id", jwt_key);
};

void DeviceConfiguration::set_device_id(const std::string& device_id) {
  this->add_value("device_id", device_id);
};
}  // namespace P2PFileSync::Serverkit