//
// Created by hanzech on 11/14/21.
//

#ifndef P2P_FILE_SYNC_INSTANCE_POOL_H
#define P2P_FILE_SYNC_INSTANCE_POOL_H
#include <mutex>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

//TODO need to complete document
namespace P2PFileSync {
template <typename KEY, typename INSTANCE>
class InstancePool {
 public:
  /**
   *
   * @tparam Args
   * @param key
   * @param args
   * @return
   */
  template <typename... Args>
  bool new_instance(const KEY& key, Args&&... args) {
    // unique allows only one instance could insert instance at same time to pool
    std::unique_lock<std::shared_mutex> sharedLock(_lock_mutex);
    if (_instance_pool.find(key) == _instance_pool.end()) {
      _instance_pool.insert(key, std::make_shared<INSTANCE>(args...));
      return true;
    } else {
      return false;
    }
  }

  bool add_instance(const KEY& key, std::shared_ptr<INSTANCE> instance) {
    // unique allows only one instance could insert instance at same time to pool
    std::unique_lock<std::shared_mutex> sharedLock(_lock_mutex);
    if (_instance_pool.find(key) == _instance_pool.end()) {
      _instance_pool.insert(std::make_pair(key, instance));
      return true;
    } else {
      return false;
    }
  }

  /**
   * @brief get instance fomr the pool
   * @note If specific key is not found in pool will return nullptr instead
   * @param key the key of thins instance
   * @return the storage instance ptr from pool if avaliabe
   */
  std::shared_ptr<INSTANCE> get_instance(const KEY& key) {
    // shared lock allows multiple thread perform search and get
    std::shared_lock<std::shared_mutex> sharedLock(_lock_mutex);
    auto ret = _instance_pool.find(key);

    if (ret == _instance_pool.end()) {
      return nullptr;
    } else {
      return ret->second;
    }
  }

 private:
  std::shared_mutex _lock_mutex;
  std::unordered_map<KEY, std::shared_ptr<INSTANCE>> _instance_pool;
};
}  // namespace P2PFileSync::Protocol
#endif  // P2P_FILE_SYNC_INSTANCE_POOL_H
