#ifndef P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_USER_GET_GROUP_INFO_RESPONSE_H
#define P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_USER_GET_GROUP_INFO_RESPONSE_H

#include <string>

#include "model/json_model.h"
#include "management_api_export.h"

namespace P2PFileSync::ManagementAPI {

class GroupInfoResponse : public JsonModel {
 public:

  GroupInfoResponse() = delete;


  explicit GroupInfoResponse(char* json);

  MANAGEMENT_API_EXPORT int get_group_id();

  MANAGEMENT_API_EXPORT std::string get_group_name();

  MANAGEMENT_API_EXPORT std::string get_group_desc();

};
}  // namespace P2PFileSync::ManagementAPI

#endif  // P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_USER_GET_GROUP_INFO_RESPONSE_H