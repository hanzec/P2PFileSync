#ifndef P2P_FILE_SYNC_PROTOCOL_PROTOCOL_H
#define P2P_FILE_SYNC_PROTOCOL_PROTOCOL_H
#include <openssl/pkcs12.h>

#include <future>
#include <cstddef>
#include <ip_addr.h>
#include <filesystem>

#include "export.h"
#include "utils/thread_pool.h"

namespace P2PFileSync::Protocol {
class ProtocolServer : private ThreadPool {
  using INSTANCE_PTR = std::shared_ptr<ProtocolServer>;

 public:
  /**
   * @brief Initial function which will need to call only once, call mutiple
   * init function will cause unexpected error. In this methods will do
   * following things:
   *    1. check the certificate_path is existed and [valid(//TODO not
   *       finished)], if not exist return false
   *    2. checking the format of listen_address, if not valid pattern then
   *       return false. Then trying to establish the listen socket, if failed
   *       return false
   *    3. By calling the protected constructor will do following thins:
   *       3.1 load certificate from disk to memory struct
   *       3.2 Trying to set up an thread where running a libevent loop to
   *           handling incoming connection
   *       3.3 create local therad pool for sending outcoming message async
   *
   * @note this function's return value is marked by nodiscard since suggest
   * caller to check the init process is success or not
   * @param number_worker the number of the worker thread
   * @param listen_address the ip address and port to listen on in
   * [xxx.xxx.xxx.xxx:xxxx] format
   * @param certificate_path the client certificate path
   * @return true the ProtocolServer init success
   * @return false the ProtocolServer init failed
   */
  [[nodiscard]] EXPORT_FUNC static bool init(
      const uint8_t number_worker, const std::string& listen_address,
      const std::filesystem::path& certificate_path);

  /**
   * @brief Get the instance of ProtocolServer, nullptr if init() not called
   *
   * @note this function's return value is marked by nodiscard since suggest
   * caller to check return instance reference is nullptr or not
   * @return std::shared_ptr<ProtocolServer> reference of ProtocolServer,
   * nullptr if init() not called
   */
  [[nodiscard]] EXPORT_FUNC static INSTANCE_PTR get_instance();

 protected:
  /**
   * @brief Provate Constructer of ProtocolServer which avoid others call
   * constructor cause multiple instance exist, see details in document of
   * init() method
   *
   * @param number_worker the number of the worker thread
   * @param certificate_path the client certificate path
   */
  ProtocolServer(const uint8_t number_worker, const int fd,
                 const std::filesystem::path& certificate_path);

 private:
  /**
   * Private instance
   */
  inline static INSTANCE_PTR _instance = nullptr;

  /**
   * Instance private variables
   */
  PKCS12* _certificate;
};
}  // namespace P2PFileSync::Protocol
#endif