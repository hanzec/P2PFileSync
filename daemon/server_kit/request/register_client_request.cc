#include "request.hpp"

class RegisterClientRequest : public ServerRequestModel{
  public:
    RegisterClientRequest() = default;

    RegisterClientRequest(char*  json):ServerRequestModel(json){};

    void setMachineID(const std::string& machine_id){
      this->addValue("machine_id", machine_id);
    }

    void setIPAddress(const std::string& ipaddress){
      this->addValue("ip_address", ipaddress);
    }
};