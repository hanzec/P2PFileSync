#include <memory>
#include <string>
#include <string_view>

#include "client_certificate_response.h"
namespace P2PFileSync::Serverkit {

ClientCertResponse::ClientCertResponse(char* json) : IJsonModel(json){};

std::string ClientCertResponse::get_client_id() {
  return get_value<std::string>("client_id");
}

std::string ClientCertResponse::get_client_PSCK12_certificate() {
  return get_value<std::string>("PSCK12_certificate");
}
}  // namespace P2PFileSync::Serverkit