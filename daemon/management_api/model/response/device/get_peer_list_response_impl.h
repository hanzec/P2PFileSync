#ifndef P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_GET_PEER_LIST_IMPL_H
#define P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_GET_PEER_LIST_IMPL_H
#include <string>

#include "model/json_model.h"
#include "model/IPeerListResponse.h"

namespace P2PFileSync::ManagementAPI {

class PeerListResponseImpl : public JsonModel, public IPeerListResponse {
 public:
  PeerListResponseImpl() = delete;

  ~PeerListResponseImpl() override = default;

  explicit PeerListResponseImpl(char* json);

  std::map<std::string, std::string> peer_list() override;
};
}  // namespace P2PFileSync::ManagementAPI

#endif  // P2P_FILE_SYNC_SERVER_KIT_MODEL_RESPONSE_GET_PEER_LIST_IMPL_H