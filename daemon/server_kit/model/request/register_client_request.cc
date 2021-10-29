#include "register_client_request.h"

namespace P2PFileSync::Serverkit {

RegisterClientRequest::RegisterClientRequest(char* json) : IJsonModel(json){};

void RegisterClientRequest::setMachineID(const std::string& machine_id) {
  this->addValue("machineID", machine_id);
};

void RegisterClientRequest::setIPAddress(const std::string& ipaddress) {
  this->addValue("ip", ipaddress);
};
}  // namespace P2PFileSync::Serverkit