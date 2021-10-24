#include "register_client_response.hpp"
#include <memory>
#include <string>
#include <string_view>

namespace P2PFileSync::Server_kit {

RegisterClientResponse::RegisterClientResponse(char* json) : IJsonModel(json){};

int RegisterClientResponse::get_register_code() {
  return get_value<int>("register_code");
}

std::string RegisterClientResponse::get_client_id() {
  return get_value<std::string>("client_id");
}

std::string RegisterClientResponse::get_enable_url() {
  return get_value<std::string>("enable_url");
}

std::string RegisterClientResponse::get_raw_psck12_certificate() {
  return get_value<std::string>("PSCK12_certificate");
}
}  // namespace P2PFileSync::Server_kit