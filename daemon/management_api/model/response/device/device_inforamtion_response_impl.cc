#include <memory>
#include "device_information_response_impl.h"
namespace P2PFileSync::ManagementAPI {

DeviceInfoResponseImpl::DeviceInfoResponseImpl(char* json) : JsonModel(json){};

std::string DeviceInfoResponseImpl::device_id()const {
  return get_value<std::string>("client_id").value();
}

std::string DeviceInfoResponseImpl::machine_id()const {
  return get_value<std::string>("machine_id").value();
}
std::string DeviceInfoResponseImpl::device_ip() const{
  return get_value<std::string>("ip_address").value();
}
std::string DeviceInfoResponseImpl::register_user() const{
  return get_value<std::string>("register_by").value();
}
}  // namespace P2PFileSync::ManagementAPI