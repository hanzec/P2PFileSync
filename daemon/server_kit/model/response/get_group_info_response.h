#ifndef P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_GET_GROUP_INFO_RESPONSE_H
#define P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_GET_GROUP_INFO_RESPONSE_H

#include <string>

#include "model/IJsonModel.h"

namespace P2PFileSync::Serverkit {

class GroupInfoResponse : public IJsonModel {
 public:

  GroupInfoResponse() = delete;


  GroupInfoResponse(char* json);

  EXPORT_FUNC int get_group_id();

  EXPORT_FUNC std::string get_group_name();

  EXPORT_FUNC std::string get_group_desc();

};
}  // namespace P2PFileSync::Serverkit

#endif  // P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_GET_GROUP_INFO_RESPONSE_H