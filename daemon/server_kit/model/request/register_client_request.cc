#include "register_client_request.hpp"

namespace P2PFileSync::Server_kit {

RegisterClientRequest::RegisterClientRequest(char* json) : IJsonModel(json){};

void RegisterClientRequest::setMachineID(const std::string& machine_id) {
  this->addValue("machineID", machine_id);
};

void RegisterClientRequest::setIPAddress(const std::string& ipaddress) {
  this->addValue("ip", ipaddress);
};
}  // namespace P2PFileSync::Server_kit