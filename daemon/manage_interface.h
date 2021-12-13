#ifndef P2P_FILE_SYNC_MANAGE_INTERFACE_H
#define P2P_FILE_SYNC_MANAGE_INTERFACE_H
#include <sys/socket.h>

#include <array>
#include <string>
#include <memory>
#include <thread>
#include <unordered_map>
#include <event2/util.h>
#include <event2/event.h>
#include <vector>

namespace  P2PFileSync{
class ManagementInterface{
 protected:
  /**
   * Blocker avoid call public constructor
   */
  struct this_is_private;

 public:
  static std::shared_ptr<ManagementInterface> init(const std::string& socket, int listen_type);

  ManagementInterface(const this_is_private&, const std::string& socket, int listen_type, int timeout = 3600);

 protected:

  static ::std::shared_ptr<ManagementInterface> get(){
    return _instance;
  }
  /**
   *
   * @tparam Args
   * @param args
   * @return
   */
  template <typename... Args>
  static ::std::shared_ptr<ManagementInterface> create(Args&&... args) {
    return ::std::make_shared<ManagementInterface>(this_is_private{0},
                                           ::std::forward<Args>(args)...);

  }
  /**
   * Blocker avoid call public constructor
   */
  struct this_is_private {
    explicit this_is_private(int) {}
  };

  static void executor_container(int new_fd, int timeout);

  static void accept_handler(evutil_socket_t fd, short event, void *arg);


 private:
  void manage_interface_thread() const;
  int _listen_socket = -1;
  const int _timeout;
  inline static std::shared_ptr<ManagementInterface> _instance{nullptr};
  std::thread _manage_interface_thread;
  std::unordered_map<int, bool> _executor_container_running_flag;
  std::unordered_map<int, std::thread> _thread_management_container;
};

/**
 * @brief Start manage interface for P2PFileSync with socket address
 *
 * @param ip_address the address and port need to listen
 */
} // P2PFileSync
#endif
