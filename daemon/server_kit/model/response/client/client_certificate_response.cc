#include <memory>
#include "client_certificate_response.h"
namespace P2PFileSync::ServerKit {

ClientCertResponse::ClientCertResponse(char* json) : IJsonModel(json){};

std::string ClientCertResponse::device_id() {
  return get_value<std::string>("client_id");
}

std::string ClientCertResponse::PSCK12_certificate() {
  return get_value<std::string>("PSCK12_certificate");
}
}  // namespace P2PFileSync::ServerKit