#ifndef P2P_FILE_SYNC_PROTOCOL_UTILS_FIFO_CACHE_H
#define P2P_FILE_SYNC_PROTOCOL_UTILS_FIFO_CACHE_H

#include <list>
#include <string>
#include <unordered_map>

namespace P2PFileSync::Protocol {
template <typename T>
class FIFOCache {
 public:
  //TODO add document
  explicit FIFOCache(size_t cache_size)
      : _cache(cache_size),
        _cache_map(cache_size),
        _max_cache_size(cache_size){};
  /**
   * @brief check T is cached or not, if not cached before will add in
   * to cache and return false, if cached will return true. Note this function
   * have a maximum handled cache size, if max cached sized is meet, will remove
   * the First added T worked as FIFO queue
   *
   * @param packet_id
   * @return true the packet id is already in queue
   * @return false
   */
  bool is_cached(const T& packet_id) {
    auto ret = _cache_map.find(packet_id);

    if (ret == _cache_map.end()) {  // if cache not exist
      if (_cache.size() == _max_cache_size) {
        // delete first element in the list
        _cache_map.erase(_cache.front());
        _cache.pop_front();
      }

      // add to end of the list
      _cache_map.insert(packet_id);
      _cache.emplace_back(packet_id);
      return false;
    } else {
      return true;
    }
  }

 private:
  const size_t _max_cache_size;
  std::list<T> _cache;
  std::unordered_set<T> _cache_map;
};
}  // namespace P2PFileSync::Protocol

#endif  // P2P_FILE_SYNC_PROTOCOL_UTILS_FIFO_CACHE_H