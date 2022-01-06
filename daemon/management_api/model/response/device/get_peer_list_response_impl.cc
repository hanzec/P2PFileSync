#include <memory>
#include "get_peer_list_response_impl.h"

namespace P2PFileSync::ManagementAPI {

PeerListResponseImpl::PeerListResponseImpl(char* json) : JsonModel(json){};

std::map<std::string,std::string> PeerListResponseImpl::peer_list() {
  return get_map<std::string>("peers");
}

}  // namespace P2PFileSync::ManagementAPI