#include "register_client_request.h"

namespace P2PFileSync::ServerKit {

RegisterClientRequest::RegisterClientRequest():IJsonModel(){
  this->add_value("group", "DEFAULT_GROUP");
}

void RegisterClientRequest::setMachineID(const std::string& machine_id) {
  this->add_value("machineID", machine_id);
};

void RegisterClientRequest::setIPAddress(const std::string& ipaddress) {
  this->add_value("ip", ipaddress);
};
}  // namespace P2PFileSync::ServerKit