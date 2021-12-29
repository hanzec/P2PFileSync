#ifndef P2P_FILE_SYNC_PROTOCOL_PROTOCOL_H
#define P2P_FILE_SYNC_PROTOCOL_PROTOCOL_H

#include <event2/event.h>
#include <event2/util.h>
#include <openssl/pkcs12.h>
#include <openssl/x509_vfy.h>
#include <protocol.pb.h>

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
#include "server_kit/server_kit.h"
#include "utils/data_struct/fifo_cache.h"
#include "utils/data_struct/instance_pool.h"
#include "utils/data_struct/routing_table.h"
#include "utils/data_struct/thread_pool.h"
#include "utils/singleton_helper.h"

namespace P2PFileSync {

using DeviceContextPtr = std::shared_ptr<ServerKit::DeviceContext>;

class P2PServerContext : public Singleton<P2PServerContext>,
                         private RoutingTable<std::string>,
                         private FIFOCache<std::basic_string<char>> {
 public:
  /**
   * @brief Delete default constructor avoid construct without calling init()
   */
  P2PServerContext() = delete;

  /**
   * @brief Provate Constructer of P2PServerContext which avoid others call
   * constructor cause multiple instance exist, see details in document of
   * init() method
   *
   * @param number_worker
   * @param fd
   * @param cert
   * @param cert_sign
   * @param packet_cache_size
   */
  P2PServerContext(const this_is_private&, uint32_t packet_cache_size,
                   std::shared_ptr<ThreadPool>& thread_pool, X509* cert, EVP_PKEY* private_key,
                   uint16_t port, STACK_OF(X509) * ca);

  /**
   * @brief Initial function which will need to call only once, call mutiple
   * init function will cause unexpected error. In this methods will do
   * following things:
   *    1. this fuction will continue block until client is activated by server
   *       side. This function will test active status once per second.
   *    2. checking the format of listen_address, if not valid pattern then
   *       return false. Then trying to establish the listen socket, if failed
   *       return false
   *    3. Loading and verify the certificate from disk
   *    4. By calling the protected constructor will do following thins:
   *       4.1 Trying to set up an thread where running a libevent loop to
   *           handling incoming connection
   *       4.2 create local therad pool for sending outcoming message async
   *       4.3 Send request to all possible cancidate for discovering other
   *           online peer
   * @note this function's return value is marked by nodiscard since suggest
   * caller to check the init process is success or not
   * @param config the client configuration class
   * @param device_context the device context get from server_kit handle_difficult the
   * realted server API
   * @return true the P2PServerContext init success
   * @return false the P2PServerContext init failed
   */
  static bool init(const std::shared_ptr<Config>& config,
                   std::shared_ptr<ThreadPool> thread_pool,
                   const DeviceContextPtr& device_context);

  // TODO need document
  std::future<bool> send_pkg(const ProtoMessage& data, const std::shared_ptr<IPAddr>& peer);

  // TODO need document
  template <typename T>
  ProtoMessage package_pkg(const T& data, const std::string& receiver);

  // TODO need document
  void block_util_server_stop();

  // TODO need document
  const std::unordered_map<std::string, std::pair<std::shared_ptr<IPAddr>, uint32_t>>&
  get_online_peers();

 protected:
  // TODO need document
  bool start(int fd);

  /**
   * Peer session for storage all known peer with their public Key
   */
  class PeerSession {
   public:
    /**
     * @brief Delete default constructor prevent construct object without calling new_session
     */
    PeerSession() = delete;

    /**
     * @brief The deconstruct will do following thins:
     *  1. free EVP_PKEY
     *  2. if EVP_MD_CTX not pointer then free
     */
    ~PeerSession();

    /**
     * @brief Construct new PeerSession object
     *  In this method , will check the incoming cert is valid by ca certificate bundle which
     * come from the client certificate
     * @param raw_cert the raw x509 certificate of the peer as bytes in string container
     * @param ca the stack of ca certificate used to verity the peer certificate
     * @return if cert valid then return std::shared_ptr<PeerSession> else return nullptr
     */
    static std::shared_ptr<PeerSession> new_session(const std::string& raw_cert) noexcept;

    /**
     * @brief verify the signature with specific data
     *  In this method, it will only create one times of EVP_MD_CTX used to verify the
     *  certificate, EVP_MD_CTX will initialized within the fist call of verify, and cached for
     * further method calling
     * @note if EVP_MD_CTX is failed to create , this function will always return
     * false.
     * @note if using unsupported method with return false then print log to ERROR level
     * @param data
     * @param sig
     * @param method the digest method in string
     * @return true for accepted digest, false for wong digest
     */
    bool verify(const std::string& data, const std::string& sig,
                const std::string& method) noexcept;

   protected:
    /**
     * @brief protected constructor for peer session
     *
     * @param _public_key
     */
    explicit PeerSession(EVP_PKEY* _public_key) noexcept;

   private:
    EVP_PKEY* _public_key;
    EVP_MD_CTX* _evp_md_ctx = nullptr;
  };

 private:
  /*
   * @brief Package utils to construct and handle the package
   *
   * @todo need change
   */
  class PacketHandler {
   public:
    static void handle(SignedProtoMessage& signed_msg, const sockaddr_in* incoming_connection);

    template <typename... Args>
    static PacketType construct(Args&&... args) {
      return construct_internal(std::forward<Args>(args)...);
    }

   protected:

    static ProtoHelloMessage construct_internal(X509* cert) noexcept;


    template <class PayloadType>
    class _handler_impl {
     public:
      static bool handle(const ProtoMessage& proto_msg,
                         const sockaddr_in* incoming_connection) noexcept {
        PayloadType payload;
        proto_msg.payload().UnpackTo(&payload);
        return internal_handle(payload, incoming_connection);
      }

     private:
      static bool internal_handle(const PayloadType& proto_msg,
                                  const sockaddr_in* incoming_connection){
        LOG(ERROR) << "handler [" << typeid(PayloadType).name() << "] not implemented !";
        return false;
      }
    };
  };

  // TODO need document
  template <typename T, typename... Args>
  static T construct_payload(Args&&... args) {
    return PacketHandler<T>::construct(std::forward<Args>(args)...);
  }

  /**
   * Private instance
   */
  inline static std::shared_ptr<ServerKit::DeviceContext> _device_context = nullptr;

  /**
   * Client Certificate //TODO delete those when deconstruct avoid leaking
   */
  X509* _client_cert;  // todo need free
  X509_STORE* _x509_store;
  EVP_MD_CTX* _evp_md_ctx = EVP_MD_CTX_new();
  EVP_PKEY* _client_priv_key = nullptr;

  /**
   * Instance private variables
   */
  const PKCS12* _certificate{};
  const uint16_t _port;
  struct event_base* _event_base;
  std::unique_ptr<std::thread> _thread_ref{nullptr};
  InstancePool<std::string, PeerSession> _peer_pool;
  std::shared_ptr<ThreadPool> _thread_pool;

  /**
   * @brief Get the event base of libevent
   * @note this symbol will not export
   * @return struct event_base*
   */
  [[nodiscard]] struct event_base* event_base() { return _event_base; }

  /**
   * @brief function contains the content of listening thread
   *
   * @param fd the established listen handler for incoming connection
   * @param protool_server pointer of packet server instance
   */
  static void listening_thread(int fd);

  /**
   * @brief When handling the imcoming packet will do following steps:
   *  1. check the validity of the packet, if the packet is corrupt or
   *     incomplete, then will directly abandon and print a message in ERROR
   *     level
   *  2. check if the packet is handled or not, by design a packet has its
   *     unique id and will not handle_difficult twice for one peer node. If packet is
   *     already handled then will return immediately without any log
   *     printed(only print log when VLOG=3)
   *  3. Checking if the packed is redirect-only(receiver is other peer) for
   *     leaving handler as early as possible. This function will check if the
   *     destination peer is reachable or unreachable by checking if there is an
   *     corresponding entry existed in routing table or not. If not reachable
   *     then will ignore the packet without any log print(will only print log
   *     in VLOG=3)、
   * @note that when handing the unsupported request type, the client will
   * ignore the packet with out any log print(will only print log in VLOG=3)
   * @param bev libevent bufferent event structure
   * @param ctx the pointer of the P2PServerContext instance
   */
  static void read_callback(struct bufferevent* bev, void* ctx);

  /**
   * @brief The libevent event callback fuction which will be called when
   * connection is ready to accept. In this function will create a libevent
   * bufferenvet for managing the read/disconnect event
   *
   * @param fd the file descriptor generated by libevent
   * @param event the event type
   * @param arg the raw pointer of P2PServerContext
   */
  static void libev_callback(evutil_socket_t fd, short event, void* arg);

  // TODO add document
  static void event_callback(struct bufferevent* bev, short events, void* ctx);
};

}  // namespace P2PFileSync
#endif