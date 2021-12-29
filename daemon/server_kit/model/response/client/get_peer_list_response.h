#ifndef P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_GET_PEER_LIST_H
#define P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_GET_PEER_LIST_H
#include <string>
#include "../../IJsonModel.h"

namespace P2PFileSync::ServerKit {

class PeerListResponse : public IJsonModel {
 public:
  PeerListResponse() = delete;


  explicit PeerListResponse(char* json);

  EXPORT_FUNC std::map<std::string,std::string> peer_list();

};
}  // namespace P2PFileSync::ServerKit

#endif  // P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_SET_PEER_LIST_H