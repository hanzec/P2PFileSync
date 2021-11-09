#include <memory>
#include <string>
#include <string_view>

#include "get_group_info_response.h"

namespace P2PFileSync::Serverkit {

GroupInfoResponse::GroupInfoResponse(char* json) : IJsonModel(json){};

int GroupInfoResponse::get_group_id() {
  return get_value<int>("\\group\\id");
}

std::string GroupInfoResponse::get_group_name() {
  return get_value<std::string>("\\group\\name");
}

std::string GroupInfoResponse::get_group_desc() {
  return get_value<std::string>("\\group\\description");
}

}  // namespace P2PFileSync::Serverkit