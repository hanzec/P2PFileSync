#ifndef P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_SET_PEER_LIST_H
#define P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_SET_PEER_LIST_H

#include <string>

#include "../IJsonModel.h"

namespace P2PFileSync::Serverkit {

class GetPeerListResponse : public IJsonModel {
 public:

  GetPeerListResponse() = delete;


  GetPeerListResponse(char* json);

  EXPORT_FUNC std::map<std::string,std::string> get_peer_list();

};
}  // namespace P2PFileSync::Serverkit

#endif  // P2P_FILE_SYNC_Serverkit_MODEL_REQUEST_SET_PEER_LIST_H