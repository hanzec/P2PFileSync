//
// Created by hanzech on 11/24/21.
//

#ifndef P2P_FILE_SYNC_ROUTING_TABLE_H
#define P2P_FILE_SYNC_ROUTING_TABLE_H
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <glog/logging.h>
#include "utils/ip_address.h"

namespace P2PFileSync {
template <typename T>
class RoutingTable {
 public:
  /**
   * @brief Get the ip address of specific destination peer
   *
   * @param dest the id of destination peer
   * @return IPAddress& the ip address of the destination peer
   */
  std::shared_ptr<IPAddress> get_next_peer(const T& dest) {
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
  bool can_delivered(const T& dest) {
    std::shared_lock<std::shared_mutex> sharedLock(_lock_mutex);

    return _routing_map.find(dest) != _routing_map.end();
  }

  // TODO add document
  bool add_new_route(const T& dest, std::shared_ptr<IPAddress> new_ip, int new_ttl) {
    std::shared_lock<std::shared_mutex> sharedLock(_lock_mutex);

    if(_routing_map.find(dest) == _routing_map.end()) {
      _routing_map.insert(std::make_pair(dest, std::make_pair(new_ip, new_ttl)));
      VLOG(3) << "Add new route: " << dest << " " << *new_ip << " " << new_ttl;
      return true;
    } else {
      VLOG(3) << "Route already exists: " << dest << " " << new_ip << " " << new_ttl;
      return false;
    }
    return true;
  }

  // TODO add document
  bool try_update_table(const T& dest, std::shared_ptr<IPAddress> new_ip, int new_ttl) {
    std::shared_lock<std::shared_mutex> sharedLock(_lock_mutex);
    auto ret = _routing_map.find(dest);
    if (ret == _routing_map.end()) {
      LOG(WARNING) << "try_update_table: destination peer not found in routing table";
      return false;
    } else {
      if (ret->second.second > new_ttl) {
        // if current routing item is upgradeable, then update the lock level
        sharedLock.unlock();
        std::unique_lock<std::shared_mutex> uniqueLock(_lock_mutex);
        if (ret->second.second > new_ttl) { // check again after lock upgrade
          ret->second.first = new_ip;
          ret->second.second = new_ttl;
          return true;
        } else {
          VLOG(3) << "try_update_table: destination peer's lock level is not upgradeable: OLD["
                  << ret->second.second << "] Vs NEW[" << new_ttl << "]";
          return false;
        }
      } else {
        VLOG(3) << "try_update_table: destination peer's lock level is not upgradeable: OLD["
                << ret->second.second << "] Vs NEW[" << new_ttl << "]";
        return false;
      }
    }
  }

  // TODO add document
  const std::unordered_map<std::string, std::pair<std::shared_ptr<IPAddress>, uint32_t>>&
  get_routing_table() const {
    return _routing_map;
  }

 private:
  std::shared_mutex _lock_mutex;
  std::unordered_map<std::string, std::pair<std::shared_ptr<IPAddress>, uint32_t>> _routing_map;
};

}  // namespace P2PFileSync
#endif  // P2P_FILE_SYNC_ROUTING_TABLE_H
