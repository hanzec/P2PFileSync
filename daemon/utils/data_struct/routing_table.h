//
// Created by hanzech on 11/24/21.
//

#ifndef P2P_FILE_SYNC_ROUTING_TABLE_H
#define P2P_FILE_SYNC_ROUTING_TABLE_H
#include <memory>
#include <mutex>
#include <shared_mutex>

#include "../ip_addr.h"

namespace P2PFileSync {
template<typename T>
class RoutingTable {
 public:
  /**
   * @brief Get the ip address of specific destination peer
   *
   * @param dest the id of destination peer
   * @return IPAddr& the ip address of the destination peer
   */
  std::shared_ptr<IPAddr> get_next_peer(const T& dest) {
    // shared lock allows multiple thread perform search and get
    std::shared_lock<std::shared_mutex> sharedLock(_lock_mutex);
    return _routing_map.find(dest)->second.first;
  }

  /**
   * @brief Check if the destination peer is existed in current routing table
   *
   * @param dest_peer_id the id of destination peer
   * @return true found the destination peer’s address in routing table
   * @return false destination peer not found in routing table
   */
  bool can_delivered(const T& dest){
    std::shared_lock<std::shared_mutex> sharedLock(_lock_mutex);

    return _routing_map.find(dest) == _routing_map.end();
  }

  //TODO add document
  bool add_new_route(const T& dest, const IPAddr& new_ip, int new_ttl){
    std::shared_lock<std::shared_mutex> sharedLock(_lock_mutex);

    return true;
  }

  //TODO add document
  bool try_update_table(const T& dest, IPAddr& new_ip, int new_ttl){
    std::shared_lock<std::shared_mutex> sharedLock(_lock_mutex);
    auto ret = _routing_map.find(dest);
    if(ret == _routing_map.end()){
      return false;
    } else {
      if(ret->second.second > new_ttl){
        // if current routing item is upgradeable, then update the lock level
        sharedLock.unlock();
        std::unique_lock<std::shared_mutex> uniqueLock(_lock_mutex);
        if(ret->second.second > new_ttl){
          ret->second.second = new_ttl;
          ret->second.first = std::make_shared<IPAddr>(std::move(new_ip));
          return true;
        } else{
          return false;
        }
      } else {
        return false;
      }
    }
  }

  //TODO add document
  const std::unordered_map<std::string, std::pair<std::shared_ptr<IPAddr>, uint32_t>>& get_routing_table() const{
    return _routing_map;
  }

 private:
  std::shared_mutex _lock_mutex;
  std::unordered_map<std::string, std::pair<std::shared_ptr<IPAddr>, uint32_t>> _routing_map;
};

}  // namespace P2PFileSync::Protocol
#endif  // P2P_FILE_SYNC_ROUTING_TABLE_H
