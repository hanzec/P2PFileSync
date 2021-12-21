#ifndef P2P_FILE_SYNC_MANAGE_INTERFACE_H
#define P2P_FILE_SYNC_MANAGE_INTERFACE_H
#include <event2/event.h>
#include <event2/util.h>
#include <sys/socket.h>

#include <array>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <shared_mutex>
#include <unordered_map>

#include "utils/singleton_helper.h"
#include "interface/connection_container.h"
namespace P2PFileSync {
/**
 * @brief The ManageInterface class
 */
class ManagementInterface : public Singleton<ManagementInterface> {
 public:

  ~ManagementInterface();

  /**
   * @brief internal constructor for constructing a new instance of management interface, the
   * caller should not use this constructor. This constructor will construct by following
   * steps:
   *    1. trying to create a socket for listening with the given socket location and
   *    listen_type
   *    2. bind the socket to the given socket location
   *    3. start listening thread with ManagementInterface::manage_interface_thread method
   *
   * @note currently only support TCP socket(AF_INET) and Unix socket(AF_UNIX). (todo) will
   *       support IPV6 in the future
   * @param this_is_private protect the caller from calling this constructor
   * @param socket The socket location the interface will listen
   * @param listen_type the type of socket which current management interface will listen
   * @param timeout  the timeout of socket, by default is 3600 seconds
   */
  ManagementInterface(const this_is_private&, const std::string& socket, int listen_type,
                      int timeout = 3600);

  /**
   * @brief wait for the management interface to be exited.
   */
  void join();

 protected:
  /**
   * @brief Libevent accept callback function for management interface, this function will be
   *        called by libevent, should not be called by any other class or function except
   *        libevent. In this method, will do the following things:
   *            1. accept the new connection
   *            2. create a new thread to handle the new connection by constructing a new
   *               ConnectionContainer class
   *            3. add the new connection to internal connection map for management
   *
   * @note the dispatch of the thread will happened before putting to container.
   * @param fd the file descriptor of the eveutil socket
   * @param event the event which is triggered by the event loop
   * @param arg the argument which is passed to the callback function
   */
  static void accept_handler(evutil_socket_t fd, short event, void* arg);

 private:
  void schedule_destroy(int fd);

  /**
   * @brief the thread function for managing interface, this function will be called by a new
   * thread. In this function, will do the following things:
   *    1. create a new event base for the thread
   *    2. dispatch the event base
   *
   * @note this function will be called by a new thread, so the caller should not call this
   */
  static void manage_interface_thread(int listen_fd) ;

  std::shared_mutex _mutex;
  event_base * _event_base;
  std::vector<int> _destroy_list;
  std::thread _manage_interface_thread;
  std::unordered_map<int, std::unique_ptr<ConnectionContainer>> _thread_management_container;
};

/**
 * @brief Start manage interface for P2PFileSync with socket address
 *
 * @param ip_address the address and port need to listen
 */
}  // namespace P2PFileSync
#endif
