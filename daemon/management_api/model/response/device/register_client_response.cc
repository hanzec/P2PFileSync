#include "register_client_response.h"
#include <memory>

namespace P2PFileSync::ManagementAPI {

RegisterClientResponse::RegisterClientResponse(char* json) : JsonModel(json){};

int RegisterClientResponse::get_register_code() {
  return get_value<int>("register_code").value();
}

std::string RegisterClientResponse::device_id() const{
  return get_value<std::string>("client_id").value();
}

std::string RegisterClientResponse::get_enable_url() const {
  return get_value<std::string>("enable_url").value();
}


std::vector<std::string> RegisterClientResponse::ip_address() const {
  return get_value<std::vector<std::string>>("ip_address").value();
}


std::string RegisterClientResponse::jwt_key() const {
  return get_value<std::string>("login_token").value();
}
}  // namespace P2PFileSync::ManagementAPI