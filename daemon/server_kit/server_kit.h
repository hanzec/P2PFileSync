#include <curl/curl.h>
#include <openssl/pkcs12.h>

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "../utils/macro.h"
#include "common.h"
#include "model/data/device_conf.h"
#include "model/response/client/client_information_response.h"
#include "model/response/client/get_peer_list_response.h"
#include "model/response/user/user_detail_response.h"
#include "utils/singleton_helper.h"

#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H

namespace P2PFileSync::ServerKit {
/**
 * @brief The cpp class for server session per connection
 *
 */
class UserContext {
 public:
  /**
   * @brief Destroy the Server Connection object
   *  This function will destroy following things:
   *    - Free the _share_handle
   */
  EXPORT_FUNC ~UserContext();

  /**
   * @brief Get the user Context object for the given server address, This method will create a
   * new CULRSH and sett
   *
   * @note ServerContext::get_usr_ctx() will return a shared_ptr<UserContext> where
   *       ServerContext will help to manage the life time of server_address.
   * @note server_address need to have longer lifetime than the returned object
   * @return const std::string& the address of the management server
   */
  EXPORT_FUNC explicit UserContext(const std::string& server_address);

  /**
   * @brief Logout the user from the server in current login session
   *
   * @note this method will do nothing if the user is not logged in
   */
  EXPORT_FUNC void logout();

  /**
   * @brief LOCALLY check the user login status, will NOT send any request to management server
   * @return true if the user is logged in
   * @return false if the user is not logged in
   */
  [[nodiscard]] EXPORT_FUNC bool is_logged_in() const;

  /**
   * @brief Send the login request to the management server
   *
   * @return std::pair<bool, std::string> the first element is true if the login is successful,
   *         the second element is the message returned from server
   */
  [[nodiscard]] EXPORT_FUNC std::pair<bool, std::string> login(const std::string& email,
                                                               const std::string& password);

  /**
   * @brief Get the user detail information from the management server
   *
   * @note this will not cached, so everytime this function is called, it will send a request
   *       to the management server
   * @return UserDetailResponse the user detail information
   */
  [[nodiscard]] EXPORT_FUNC std::unique_ptr<UserDetailResponse> user_detail() const;

 private:
  bool _login = false;
  CURLSH* _share_handle;
  bool strict_security_ = true;
  const std::string& _server_address;
};

/**
 * @brief User to manage the peer device related requests, this class is a singleton class.
 */
class EXPORT_FUNC DeviceContext : std::enable_shared_from_this<DeviceContext> {
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
   * @brief return the enabled status at remote management server, in this
   *        function will call device_info() to test the response will return
   *        errors or not, if downloaded reponse is contains a valid json format with
   *        success and its value is true than we can say the client is enabled
   * @return true this client is enabled at management server
   * @return false this client is not enabled at management server
   */
  [[nodiscard]] EXPORT_FUNC bool is_enabled() const;

  /**
   * @brief Get the id of current peer
   * @note this will return the local cache of the id loaded from the Device configuration
   *       file, since the device id will never change after the device is registered
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
   *        return the path of current client certificate. If the certificate is already
   *        downloaded and non-corrupt, this method will reutrn the path of the certificate
   *        without request to management server
   *
   * @note if the certificate is failed to download, this method will retry
   *       MAX_DOWNLOAD_RETIRES times, and wait DOWNLOAD_RETRY_SECOND second each filed
   *       download. After MAX_DOWNLOAD_RETIRES is achieved, this method will throw an FATAL
   *       exception which will cause the program to exit.
   * @return std::filesystem::path the path of PKCS12 Certificate
   */
  [[nodiscard]] EXPORT_FUNC std::filesystem::path client_certificate() const;

  /**
   * @brief Returned current client register information from remote management
   *        server
   * @note this method will always send request to server each time.
   * @return DeviceInfoResponse the response struct contains information
   */
  [[nodiscard]] EXPORT_FUNC std::unique_ptr<DeviceInfoResponse> device_info() const;

 protected:
  friend class ServerContext;

  /**
   * @brief Construct a new Device Context by following steps:
   *
   * Existing Problem:
   *  1. (todo) JWT renew in not implemented, currently server are setting expire
   *     date as 1 year after to avoid problem
   */
  DeviceContext(const std::shared_ptr<DeviceConfiguration>& dev_conf,
                std::string& server_address, std::filesystem::path& client_configuration_path,
                std::filesystem::path& client_certificate_path);

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
  const std::filesystem::path& _client_configuration_path;
};

/**
 * @brief User to manage the server related requests, this class is a singleton. The
 *        ServerContext will keep the management server address and help to get the UserContext
 *        and DeviceContext.
 */
class EXPORT_FUNC ServerContext : public Singleton<ServerContext> {
 public:
  /**
   * @brief make sure this object only construct and get by init()
   */
  ServerContext() = delete;
  ServerContext(ServerContext&) = delete;
  ServerContext(ServerContext&&) = delete;
  ServerContext& operator=(ServerContext&) = delete;
  ServerContext& operator=(ServerContext&&) = delete;

  /**
   * @brief Construct a new Server Context object.
   *
   * @note this method will be only called by init()
   * @param config config of the current program
   * @param this_is_private protect the caller from calling this constructor
   */
  ServerContext(const this_is_private&, const std::shared_ptr<Config>& config) noexcept;

  /**
   * @brief Get an new instance of UserContext
   *
   * @return UserContext the instance of UserContext
   */
  [[nodiscard]] static EXPORT_FUNC UserContext get_usr_ctx() noexcept;

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
   * @brief Regist current device to remote management server
   * // TODO need to write code handle_difficult jwt renew
   * @return std::pair<std::string,std::string> will return the JWT Token
   * authorized by management server after register and the register id by
   * formate of <JWT Token, Peer ID>
   */
  [[nodiscard]] static std::shared_ptr<DeviceConfiguration> register_client();

 private:
  uint16_t _p2p_listen_port;
  std::string _server_address;
  std::string _p2p_listen_interface;
  std::filesystem::path _client_configuration_path;
  std::filesystem::path _client_certificate_path;
  std::filesystem::path _server_certifiacte_path;

  // instance ptr
  std::shared_ptr<DeviceContext> _device_ctx_instance = nullptr;
};
}  // namespace P2PFileSync::ServerKit

#endif  // P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H