#include <memory>
#include <string>
#include <string_view>

#include "register_client_response.h"
namespace P2PFileSync::Serverkit {

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

std::string RegisterClientResponse::get_login_token() {
  return get_value<std::string>("login_token");
}
}  // namespace P2PFileSync::Serverkit