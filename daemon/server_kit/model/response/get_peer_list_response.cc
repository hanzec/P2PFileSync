#include <memory>
#include <string>
#include <string_view>

#include "get_peer_list_response.h"

namespace P2PFileSync::Serverkit {

GetPeerListResponse::GetPeerListResponse(char* json) : IJsonModel(json){};

std::unordered_map<std::string,std::string> GetPeerListResponse::get_peer_list() {
  return get_map<std::string>("\\peer");
}

}  // namespace P2PFileSync::Serverkit