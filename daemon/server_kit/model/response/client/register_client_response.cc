#include "register_client_response.h"

#include <utils/uuid_utils.h>

#include <memory>
#include <string>
#include <string_view>

#include "utils/uuid_utils.h"
namespace P2PFileSync::ServerKit {

RegisterClientResponse::RegisterClientResponse(char* json) : IJsonModel(json){};

int RegisterClientResponse::get_register_code() {
  return get_value<int>("register_code");
}

std::string RegisterClientResponse::device_id() const{
  return get_value<std::string>("client_id");
}

std::string RegisterClientResponse::get_enable_url() const {
  return get_value<std::string>("enable_url");
}

std::string RegisterClientResponse::jwt_key() const {
  return get_value<std::string>("login_token");
}
}  // namespace P2PFileSync::ServerKit