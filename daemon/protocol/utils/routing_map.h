//
// Created by hanzech on 11/24/21.
//

#ifndef P2P_FILE_SYNC_ROUTING_MAP_H
#define P2P_FILE_SYNC_ROUTING_MAP_H
#include <ip_addr.h>

namespace P2PFileSync::Protocol {
template<typename T>
class RoutingMap {
 public:
  /**
   * @brief Get the ip address of specific destination peer
   *
   * @param dest the id of destination peer
   * @return IPAddr& the ip address of the destination peer
   */
  IPAddr& get_next_peer(const T& dest) {

  }

  /**
   * @brief Check if the destination peer is existed in current routing table
   *
   * @param dest_peer_id the id of destination peer
   * @return true found the destination peer’s address in routing table
   * @return false destination peer not found in routing table
   */
  bool can_delivered(const std::string& dest_peer_id) const{

  }

  bool add_new_route(const T&, IPAddr new_ip, int new_ttl){

  }

  bool try_update_table(const T&, IPAddr new_ip, int new_ttl){

  }


 private:
  std::unordered_map<std::string, std::pair<IPAddr, uint32_t>> _routing_map;
};

}  // namespace P2PFileSync::Protocol
#endif  // P2P_FILE_SYNC_ROUTING_MAP_H
