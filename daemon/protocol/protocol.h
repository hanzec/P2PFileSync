#ifndef P2P_FILE_SYNC_PROTOCOL_PROTOCOL_H
#define P2P_FILE_SYNC_PROTOCOL_PROTOCOL_H
#include <event2/event.h>
#include <event2/util.h>
#include <ip_addr.h>
#include <openssl/pkcs12.h>
#include <openssl/x509_vfy.h>

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <future>
#include <list>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include "common.h"
#include "export.h"
#include "hello_message.pb.h"
#include "message.pb.h"
#include "protocol/utils/fifo_cache.h"
#include "server_kit/server_kit.h"
#include "utils/thread_pool.h"

namespace P2PFileSync::Protocol {
class ProtocolServer : private ThreadPool, FIFOCache<std::string> {
  using INSTANCE_PTR = std::shared_ptr<ProtocolServer>;

 public:
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

 protected:
  /**
   * @brief Provate Constructer of ProtocolServer which avoid others call
   * constructor cause multiple instance exist, see details in document of
   * init() method
   *
   * @param number_worker
   * @param fd
   * @param cert
   * @param cert_sign
   * @param packet_cache_size
   */
  ProtocolServer(const uint8_t number_worker, const int fd, const PKCS12* cert,
                 const PKCS7* cert_sign, const uint32_t packet_cache_size);

  /**
   * @brief Handle the incoming packages
   *
   * @param message the protobuf message of incoming package
   */
  void handle(const ProtoMessage& message);

  /**
   * @brief Get the peer id object
   *
   * @return  std::string peer if
   */
  const std::string& get_peer_id() const;

  /**
   * @brief Check if the destination peer is existed in current routing table
   *
   * @param dest_peer_id the id of destination peer
   * @return true found the destination peer’s address in routing table
   * @return false destination peer not found in routing table
   */
  bool in_routing_table(std::string dest_peer_id) const;

  /**
   * @brief Get the ip address of specific destination peer
   *
   * @param dest_peer_id the id of destination peer
   * @return IPAddr the ip address of the destination peer
   */
  IPAddr& get_ip_addr_from_table(std::string dest_peer_id) const;

  /**
   * @brief send message to all avaliable peer with specific raw data and size
   * @note this function will not check the validity of the raw data.
   * @note this function will only dispatch the job but not wating until job's
   * finished execution
   * @param data the raw pointer of the data
   * @param size the size of raw pointer of the data
   * @return std::future<bool> indicates the package is successfully send or not
   */
  void broadcast_raw_package(const void*& data, const uint32_t& size);

  /**
   * @brief send message to specific peer with specific raw data and size
   * @note this function will not check the validity of the raw data, which
   * means the returned bool only indicates the peer is success send the
   * packet or not
   * @param data the raw pointer of the data
   * @param size the size of raw pointer of the data
   * @param client the destination of the the message will be sent to
   * @return std::future<bool> indicates the package is successfully send or not
   */
  std::future<bool> send_raw_package(const void* data, const uint32_t& size, const IPAddr& peer);

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

  /**
   * @brief When handling the imcoming packet will do following steps:
   *  1. check the validity of the packet, if the packet is corrupt or
   *     incomplete, then will directly abandon and print a message in ERROR
   *     level
   *  2. check if the packet is handled or not, by design a packet has its
   *     unique id and will not handle twice for one peer node. If packet is
   *     already handled then will return immediately without any log
   *     printed(only print log when VLOG=3)
   *  3. Checking if the packed is redirect-only(receiver is other peer) for
   *     leaving handler as early as possible. This function will check if the
   *     destination peer is reachable or unreachable by checking if there is an
   *     coresponding entry existed in routing table or not. If not reachable
   *     then will ignore the packet without any log print(will only print log
   *     in VLOG=3)、
   * @note that when handing the unsupported request type, the client will
   * ignore the packet with out any log print(will only print log in VLOG=3)
   * @param bev libevent bufferent event structure
   * @param ctx the pointer of the ProtocolServer instance
   */
  static void read_callback(struct bufferevent* bev, void* ctx);

  /**
   * @brief The libevent event callback fuction which will be called when
   * connection is ready to accept. In this function will create a libevent
   * bufferenvet for managing the read/disconnect event
   *
   * @param fd the file descripter generated by libevent
   * @param event the event type
   * @param arg the raw pointer of ProtocolServer
   */
  static void libev_callback(evutil_socket_t fd, short event, void* arg);

  // TODO add document
  static void event_callback(struct bufferevent* bev, short events, void* ctx);

  /**
   * @brief Internal function for handling the specific type of the handler,
   * which will need to implemented for all type of packet
   *
   * @tparam T the proto packet type
   * @param server the protocol server pointer
   * @param peer_session the peer session used to verify the package source
   * @param message the proto object fot the packet
   */
  template <typename T>
  static void handle_packet(ProtocolServer& server,
                            const PeerSession& peer_session, const T&& message);
};

}  // namespace P2PFileSync::Protocol
#endif