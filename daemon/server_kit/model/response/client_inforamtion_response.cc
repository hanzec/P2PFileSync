#include <memory>
#include <string>
#include <string_view>

#include "client_information_response.h"
namespace P2PFileSync::Serverkit {

ClientInfoResponse::ClientInfoResponse(char* json) : IJsonModel(json){};

std::string ClientInfoResponse::get_client_id() {
  return get_value<std::string>("client_id");
}

std::string ClientInfoResponse::get_client_machine_id() {
  return get_value<std::string>("machine_id");
}
std::string ClientInfoResponse::get_client_ip_address() {
  return get_value<std::string>("ip_address");
}
std::string ClientInfoResponse::get_client_register_by() {
  return get_value<std::string>("register_by");
}
}  // namespace P2PFileSync::Serverkit