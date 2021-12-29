#include <memory>
#include "get_peer_list_response.h"

namespace P2PFileSync::ServerKit {

PeerListResponse::PeerListResponse(char* json) : IJsonModel(json){};

std::map<std::string,std::string> PeerListResponse::peer_list() {
  return get_map<std::string>("peers");
}

}  // namespace P2PFileSync::ServerKit