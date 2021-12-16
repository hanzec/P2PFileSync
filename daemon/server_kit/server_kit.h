#include <curl/curl.h>
#include <openssl/pkcs12.h>

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "common.h"
#include "export.h"
#include "model/data/device_conf.h"
#include "model/response/client/client_information_response.h"
#include "model/response/client/get_peer_list_response.h"
#include "model/response/user/user_detail_response.h"

#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H

namespace P2PFileSync::ServerKit {
/**
 * @brief The cpp class for server session per connection
 *
 */
class UserContext {
 protected:
  /**
   * Blocker avoid call public constructor
   */
  struct this_is_private;

 public:
  /**
   * @brief Destroy the Server Connection object
   *  This function will destroy following things:
   *    - Free the _share_handle
   */
  EXPORT_FUNC ~UserContext();

  UserContext(const this_is_private&, std::string& _server_address);

  EXPORT_FUNC void logout();

  [[nodiscard]] EXPORT_FUNC bool is_logged_in() const;

  [[nodiscard]] EXPORT_FUNC std::unique_ptr<UserDetailResponse> user_detail() const;

  EXPORT_FUNC bool login(const std::string& email, const std::string& password);

 protected:
  friend class ServerContext;

  friend class ServerContext;

  /**
   *
   * @tparam Args
   * @param args
   * @return
   */
  template <typename... Args>
  static ::std::shared_ptr<UserContext> create(Args&&... args) {
    return ::std::make_shared<UserContext>(this_is_private{0}, ::std::forward<Args>(args)...);
  }

  /**
   * Blocker avoid call public constructor
   */
  struct this_is_private {
    explicit this_is_private(int) {}
  };

 private:
  bool _login = false;
  bool strict_security_ = true;
  CURLSH* _share_handle;
  std::string _username;
  std::string& _server_address;
};

/**
 * @brief User to manage the peer device related requests
 */
class EXPORT_FUNC DeviceContext {
 protected:
  /**
   * Blocker avoid call public constructor
   */
  struct this_is_private;

 public:
  /**
   * @brief make sure this object only construct and get by init_dev_ctx()
   */
  DeviceContext() = delete;
  DeviceContext(DeviceContext&) = delete;
  DeviceContext(DeviceContext&&) = delete;
  DeviceContext& operator=(DeviceContext&) = delete;
  DeviceContext& operator=(DeviceContext&&) = delete;

  /**
   * @brief Construct a new Device Context by following steps:
   *
   * Existing Problem:
   *  1. JWT renew in not implentmented, currently server are setting expire
   *     date as 1 year after to avoid problem
   */
  DeviceContext(const this_is_private&, const std::shared_ptr<DeviceConfiguration>& dev_conf,
                std::string& server_address, std::filesystem::path& conf,
                std::filesystem::path& client_cert_path);

  /**
   * @brief return the enabled status at remote management server, in this
   * function will call device_info() to test the response will return
   * errors or not, if downloaded reponse is contains a valid json format with
   * success and its value is true than we can say the client is enabled
   * @return true this client is enabled at management server
   * @return false this client is not enabled at management server
   */
  [[nodiscard]] EXPORT_FUNC bool is_enabled() const;

  /**
   * @brief Get the id of current peer
   * // TODO need to update document here
   * @return const std::string& the client id as string of the object
   */
  [[nodiscard]] EXPORT_FUNC const std::string& device_id() const;

  /**
   * @brief Returned the neighbor peer list from management server
   * @note this function will always send request to server each time.
   * @return std::unordered_map<std::string,std::string> list of peer
   * cancidate's ip id and its ip address
   */
  [[nodiscard]] EXPORT_FUNC std::unique_ptr<PeerListResponse> peer_list() const;

  /**
   * @brief Request PKCS12 certificate of current client, this function will
   * return the path pf current client certificate
   *
   * @return std::string return the path of PKCS12 Certificate
   */
  [[nodiscard]] EXPORT_FUNC std::filesystem::path client_certificate() const;

  /**
   * @brief Returned current client register information from remote management
   * server
   *
   * @return DeviceInfoResponse the response struct contains information
   */
  [[nodiscard]] EXPORT_FUNC std::unique_ptr<DeviceInfoResponse> device_info() const;

 protected:
  friend class ServerContext;

  /**
   *
   * @tparam Args
   * @param args
   * @return
   */
  template <typename... Args>
  static ::std::shared_ptr<DeviceContext> create(Args&&... args) {
    return ::std::make_shared<DeviceContext>(this_is_private{0},
                                             ::std::forward<Args>(args)...);
  }

  /**
   * Blocker avoid call public constructor
   */
  struct this_is_private {
    explicit this_is_private(int) {}
  };

 private:
  /**
   * Client information
   */
  const std::string _device_id;
  const std::string _login_token;

  /**
   * Configuration
   */
  const std::string& _server_address;
  const std::filesystem::path& _client_certificate_path;
  const std::filesystem::path& _server_configuration_path;
};

class EXPORT_FUNC ServerContext {
 protected:
  /**
   * Blocker avoid call public constructor
   */
  struct this_is_private;

 public:
  ServerContext(const this_is_private&, const std::shared_ptr<Config>& config) noexcept;

  EXPORT_FUNC static void init(const std::shared_ptr<Config>& config);

  [[nodiscard]] EXPORT_FUNC static std::shared_ptr<ServerContext> get_server_ctx();

  // todo add documentation
  [[nodiscard]] static EXPORT_FUNC std::shared_ptr<UserContext> get_usr_ctx() noexcept;

  /**
   * @brief initialized instance of the DeviceContext if not previous created by following
   * steps:
   *  1. check with _instance is nullptr or not if already initialized then will
   *     return the initialized instance, if not do following step
   *  2. call static method is_registered() to check if current device is
   *     registered, if not register will goto step 3, otherwise read the client
   *     config and get saved JWT Token
   *  3. call protected static method register_client to register current device
   *     and get JWT Login Token, then write all of client information and token
   *     to client.cfg
   * If DeviceContext is already created then will return the initialized instance
   * @param server_address
   * @param client_conf
   * @return true success initialized device context
   * @return false failed to initialized device context
   */
  [[nodiscard]] static EXPORT_FUNC std::shared_ptr<DeviceContext> get_dev_ctx() noexcept;

 protected:
  /**
   *
   * @tparam Args
   * @param args
   * @return
   */
  template <typename... Args>
  static ::std::shared_ptr<ServerContext> create(Args&&... args) {
    return ::std::make_shared<ServerContext>(this_is_private{0},
                                             ::std::forward<Args>(args)...);
  }

  /**
   * @brief Regist current device to remote management server
   * // TODO need to write code handle_difficult jwt renew
   * @return std::pair<std::string,std::string> will return the JWT Token
   * authorized by management server after register and the register id by
   * formate of <JWT Token, Peer ID>
   */
  [[nodiscard]] static std::shared_ptr<DeviceConfiguration> register_client();

  /**
   * Blocker avoid call public constructor
   */
  struct this_is_private {
    explicit this_is_private(int) {}
  };

 private:
  /*
   * ServerContext instance
   */
  inline static std::shared_ptr<ServerContext> _instance{nullptr};

  uint16_t _p2p_listen_port;
  std::string _server_address;
  std::string _p2p_listen_interface;
  std::filesystem::path _configuration_path;
  std::filesystem::path _client_certificate_path;
  std::filesystem::path _client_configuration_path;

  // instance ptr
  std::shared_ptr<DeviceContext> _device_ctx_instance = nullptr;
};
}  // namespace P2PFileSync::ServerKit

#endif  // P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H