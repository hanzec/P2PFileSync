//
// Created by hanzech on 12/18/21.
//

#ifndef P2P_FILE_SYNC_SINGLETON_HELPER_H
#define P2P_FILE_SYNC_SINGLETON_HELPER_H
#include <glog/logging.h>

#include <memory>

#include "management_api_export.h"

namespace P2PFileSync {
template <typename T>
class Singleton {
 public:
  template <typename... Args>
  MANAGEMENT_API_EXPORT static std::shared_ptr<T> init(Args &&...args) {
    if (_instance == nullptr) {
      return create(std::forward<Args>(args)...);
    } else {
      VLOG(1) << "singleton class for " << typeid(T).name() << " has been initialized";
      return _instance;
    }
  }

  /**
   * @brief get the created instance current Singleton class
   *
   * @note nullptr will be returned if the init() is not called
   * @return std::shared_ptr<T> instance of Singleton class
   */
  MANAGEMENT_API_EXPORT static ::std::shared_ptr<T> get() { return _instance; }

 protected:
  /**
   * @brief
   * @tparam Args
   * @param args
   * @return
   */
  template <typename... Args>
  static ::std::shared_ptr<T> create(Args &&...args) {
    if (_instance == nullptr) {
      VLOG(3) << "init singleton class for " << typeid(T).name();
      return _instance =
                 ::std::make_shared<T>(this_is_private{0}, std::forward<Args>(args)...);
    } else {
      VLOG(1) << "singleton class for " << typeid(T).name() << " has been initialized";
      return _instance;
    }
  }

  /**
   * Blocker avoid call public constructor
   */
  struct MANAGEMENT_API_EXPORT this_is_private {
    explicit this_is_private(int) {}
  };

 private:
  inline static ::std::shared_ptr<T> _instance{nullptr};
};
}  // namespace P2PFileSync
#endif  // P2P_FILE_SYNC_SINGLETON_HELPER_H
