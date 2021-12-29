#include <memory>
#include "client_information_response.h"
namespace P2PFileSync::ServerKit {

DeviceInfoResponse::DeviceInfoResponse(char* json) : IJsonModel(json){};

std::string DeviceInfoResponse::device_id() {
  return get_value<std::string>("client_id");
}

std::string DeviceInfoResponse::machine_id() {
  return get_value<std::string>("machine_id");
}
std::string DeviceInfoResponse::device_ip() {
  return get_value<std::string>("ip_address");
}
std::string DeviceInfoResponse::register_user() {
  return get_value<std::string>("register_by");
}
}  // namespace P2PFileSync::ServerKit