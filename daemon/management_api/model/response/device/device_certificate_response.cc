#include <memory>
#include "device_certificate_response.h"
namespace P2PFileSync::ManagementAPI {

ClientCertResponse::ClientCertResponse(char* json) : JsonModel(json){};

std::string ClientCertResponse::device_id() {
  return get_value<std::string>("client_id").value();
}

std::string ClientCertResponse::PSCK12_certificate() {
  return get_value<std::string>("PSCK12_certificate").value();
}
}  // namespace P2PFileSync::ManagementAPI