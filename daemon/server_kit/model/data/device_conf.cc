#include "device_conf.h"


namespace P2PFileSync::Serverkit {
void DeviceConfiguration::set_jwt_key(const std::string& jwt_key) {
  this->addValue("jwt_id", jwt_key);
};

void DeviceConfiguration::set_device_id(const std::string& device_id) {
  this->addValue("device_id", device_id);
};
}  // namespace P2PFileSync::Serverkit