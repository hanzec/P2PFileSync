#ifndef P2P_FILE_SYNC_PROTOCOL_PROTOCOL_H
#define P2P_FILE_SYNC_PROTOCOL_PROTOCOL_H
#include <event2/event.h>
#include <event2/util.h>
#include <ip_addr.h>
#include <openssl/pkcs12.h>
#include <openssl/x509_vfy.h>

#include <cstddef>
#include <filesystem>
#include <future>
#include <thread>
#include <unordered_map>

#include "common.h"
#include "export.h"
#include "server_kit/server_kit.h"
#include "utils/thread_pool.h"

namespace P2PFileSync::Protocol {
class ProtocolServer : private ThreadPool {
  using INSTANCE_PTR = std::shared_ptr<ProtocolServer>;

 public:
  /**
   * @brief Initial function which will need to call only once, call mutiple
   * init function will cause unexpected error. In this methods will do
   * following things:
   *    1. this fuction will continue block until client is activated by server
   *       side. This function will test active status once per second.
   *    1. checking the format of listen_address, if not valid pattern then
   *       return false. Then trying to establish the listen socket, if failed
   *       return false
   *    2. By calling the protected constructor will do following thins:
   *       2.1 Trying to set up an thread where running a libevent loop to
   *           handling incoming connection
   *       2.2 create local therad pool for sending outcoming message async
   *       2.3 Send request to all possible cancidate for discovering other
   * online peer
   * @note this function's return value is marked by nodiscard since suggest
   * caller to check the init process is success or not
   * @param config the client configuration class
   * @param device_context the device context get from server_kit handle the
   * realted server API
   * @return true the ProtocolServer init success
   * @return false the ProtocolServer init failed
   */
  [[nodiscard]] EXPORT_FUNC static bool init(
      const Config& config, Serverkit::DeviceContext& device_context);

  /**
   * @brief Get the instance of ProtocolServer, nullptr if init() not called
   *
   * @note this function's return value is marked by nodiscard since suggest
   * caller to check return instance reference is nullptr or not
   * @return std::shared_ptr<ProtocolServer> reference of ProtocolServer,
   * nullptr if init() not called
   */
  [[nodiscard]] EXPORT_FUNC static INSTANCE_PTR get_instance();

  /**
   * @brief send hello message to client
   *
   * @param client address of the hello message wants to send
   * @return std::future<bool> the future object returned by threaded pool which
   * indicates the hello message is success send or not
   */
  EXPORT_FUNC std::future<bool> send_hello(const IPAddr& client);

  /**
   * @brief Storage
   *
   */
  class PeerSession {
   public:
    PeerSession(const ProtocolServer& instance);

    ~PeerSession();

    bool verify(const std::string& data, const std::string& sig,
                const std::string& method);

   private:
    bool _verified = false;
    EVP_PKEY* _certificate;
    const ProtocolServer& _instance;
  };

 protected:
  /**
   * @brief Provate Constructer of ProtocolServer which avoid others call
   * constructor cause multiple instance exist, see details in document of
   * init() method
   *
   * @param number_worker the number of the worker thread
   * @param certificate_path the client certificate path
   */
  ProtocolServer(const uint8_t number_worker, const int fd, const PKCS12* cert,
                 const PKCS7* cert_sign);

 private:
  /**
   * Private instance
   */
  inline static INSTANCE_PTR _instance = nullptr;
  static Serverkit::DeviceContext& _device_context;

  /**
   * Instance private variables
   */
  const PKCS7* _sign;
  const PKCS12* _certificate;
  const std::thread _thread_ref;
  struct event_base* _event_base;
  std::unordered_map<std::string, IPAddr> routing_map;

  /**
   * @brief Get the event base of libevent
   * @note this symbol will not export
   * @return struct event_base*
   */
  [[nodiscard]] struct event_base* get_event_base() { return _event_base; }

  /**
   * @brief function contains the content of listening thread
   *
   * @param fd the established listen handler for incoming connection
   * @param protool_server pointer of protocol server instance
   */
  static void listening_thread(int fd, ProtocolServer& protool_server);
};
}  // namespace P2PFileSync::Protocol
#endif