#include <memory>
#include "get_group_info_response.h"

namespace P2PFileSync::ManagementAPI {

GroupInfoResponse::GroupInfoResponse(char* json) : JsonModel(json){};

int GroupInfoResponse::get_group_id() {
  return get_value<int>("\\group\\id").value();
}

std::string GroupInfoResponse::get_group_name() {
  return get_value<std::string>("\\group\\name").value();
}

std::string GroupInfoResponse::get_group_desc() {
  return get_value<std::string>("\\group\\description").value();
}

}  // namespace P2PFileSync::ManagementAPI