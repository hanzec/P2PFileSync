#ifndef P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_USER_GET_GROUP_INFO_RESPONSE_H
#define P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_USER_GET_GROUP_INFO_RESPONSE_H

#include <string>

#include "model/IJsonModel.h"

namespace P2PFileSync::ServerKit {

class GroupInfoResponse : public IJsonModel {
 public:

  GroupInfoResponse() = delete;


  explicit GroupInfoResponse(char* json);

  EXPORT_FUNC int get_group_id();

  EXPORT_FUNC std::string get_group_name();

  EXPORT_FUNC std::string get_group_desc();

};
}  // namespace P2PFileSync::ServerKit

#endif  // P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_USER_GET_GROUP_INFO_RESPONSE_H