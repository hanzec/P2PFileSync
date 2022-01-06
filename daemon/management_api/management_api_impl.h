#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H

#include <curl/curl.h>
#include "include/management_api.h"
#include <openssl/pkcs12.h>

#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include "model/data/device_conf.h"

namespace P2PFileSync::ManagementAPI {
/**
 * @brief The cpp class for server session per connection
 *
 */
class UserContextImpl : public IUserContext {
 public:
  /**
   * @brief Destroy the Server Connection object
   *  This function will destroy following things:
   *    - Free the _share_handle
   */
  ~UserContextImpl() override;

  /**
   * @brief Get the user Context object for the given server address, This method will create a
   * new CULRSH and sett
   *
   * @note ManagementContext_internal::get_usr_ctx() will return a shared_ptr<_UserContext>
   * where ManagementContext_internal will help to manage the life time of server_address.
   * @note server_address need to have longer lifetime than the returned object
   * @return const std::string& the address of the management server
   */
  explicit UserContextImpl(const std::string& server_address);

  /**
   * @brief Logout the user from the server in current login session
   *
   * @note this method will do nothing if the user is not logged in
   */
  void logout() override;

  /**
   * @brief LOCALLY check the user login status, will NOT send any request to management server
   * @return true if the user is logged in
   * @return false if the user is not logged in
   */
  [[nodiscard]] bool is_logged_in() const override;

  /**
   * @brief Send the login request to the management server
   *
   * @note not that on the server part there is an password length limit of maximum 20
   *       character and 8 minimum, for password length doesn't fit this requirement will
   *       directly return failed with the message: "password is to long/short"
   * @note not that on the server part there is an email length limit of maximum 20
   *       character and 8 minimum, for email length doesn't fit this requirement will
   *       directly return failed with the message: "email is to long/short"
   * @return std::pair<bool, std::string> the first element is true if the login is successful,
   *         the second element is the message returned from server
   */
  [[nodiscard]] std::pair<bool, std::string> login(const std::string& email,
                                                   const std::string& password) override;

  /**
   * @brief Get the user detail information from the management server
   *
   * @note this will not cached, so everytime this function is called, it will send a request
   *       to the management server
   * @return UserDetailResponseImpl the user detail information
   */
  [[nodiscard]] std::unique_ptr<IUserDetailResponse> user_detail() const override;

 private:
  bool _login = false;
  CURLSH* _share_handle;
  const std::string& _server_address;
};

/**
 * @brief User to manage the peer device related requests, this class is a singleton class.
 */
class DeviceContextImpl : public IDeviceContext {
 public:
  /**
   * @brief make sure this object only construct and get by init_dev_ctx()
   */
  DeviceContextImpl() = delete;
  DeviceContextImpl(DeviceContextImpl&) = delete;
  DeviceContextImpl(DeviceContextImpl&&) = delete;
  DeviceContextImpl& operator=(DeviceContextImpl&) = delete;
  DeviceContextImpl& operator=(DeviceContextImpl&&) = delete;

  /**
   * @brief return the enabled status at remote management server, in this
   *        function will call device_info() to test the response will return
   *        errors or not, if downloaded reponse is contains a valid json format with
   *        success and its value is true than we can say the device is enabled
   * @return true this device is enabled at management server
   * @return false this device is not enabled at management server
   */
  [[nodiscard]] bool is_enabled() const override;

  /**
   * @brief Get the id of current peer
   * @note this will return the local cache of the id loaded from the Device configuration
   *       file, since the device id will never change after the device is registered
   * @return const std::string& the device id as string of the object
   */
  [[nodiscard]] const std::string& device_id() const override;

  /**
   * @brief Request PKCS12 certificate of current device, this function will
   *        return the path of current device certificate. If the certificate is already
   *        downloaded and non-corrupt, this method will reutrn the path of the certificate
   *        without request to management server
   *
   * @note if the certificate is failed to download, this method will retry
   *       MAX_DOWNLOAD_RETIRES times, and wait DOWNLOAD_RETRY_SECOND second each filed
   *       download. After MAX_DOWNLOAD_RETIRES is achieved, this method will throw an FATAL
   *       exception which will cause the program to exit.
   * @return std::filesystem::path the path of PKCS12 Certificate
   */
  [[nodiscard]] std::filesystem::path client_certificate() const override;

  /**
   * @brief Returned the neighbor peer list from management server
   * @note this function will always send request to server each time.
   * @return std::unordered_map<std::string,std::string> list of peer
   * cancidate's ip id and its ip address
   */
  [[nodiscard]] std::unique_ptr<IPeerListResponse> peer_list() const override;

  /**
   * @brief Returned current device register information from remote management
   *        server
   * @note this method will always send request to server each time.
   * @return DeviceInfoResponseImpl the response struct contains information
   */
  [[nodiscard]] std::unique_ptr<IDeviceInfoResponse> device_info() const override;

 protected:
  friend class ManagementContext;

  /**
   * @brief Construct a new Device Context by following steps:
   *
   * Existing Problem:
   *  1. (todo) JWT renew in not implemented, currently server are setting expire
   *     date as 1 year after to avoid problem
   */
  DeviceContextImpl(const std::shared_ptr<DeviceConfiguration>& dev_conf,
                         std::string& server_address,
                         std::filesystem::path& client_configuration_path,
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
 *        ManagementContext_internal will keep the management server address and help to get
 * the _UserContext and _DeviceContext.
 */
class ManagementContext : std::enable_shared_from_this<ManagementContext> {
 public:
  /**
   * @brief make sure this object only construct and get by init()
   */
  ManagementContext() = delete;
  ManagementContext(ManagementContext&) = delete;
  ManagementContext(ManagementContext&&) = delete;
  ManagementContext& operator=(ManagementContext&) = delete;
  ManagementContext& operator=(ManagementContext&&) = delete;

  /**
   * @brief Construct a new Server Context object.
   *
   * @note this method will be only called by init()
   * @param config config of the current program
   * @param this_is_private protect the caller from calling this constructor
   */
  explicit ManagementContext(const Config& config) noexcept;

  /**
   * @brief Get an new instance of _UserContext
   *
   * @return _UserContext the instance of _UserContext
   */
  [[nodiscard]] static std::unique_ptr<IUserContext> usr_ctx() noexcept;

  /**
   * @brief initialized instance of the _DeviceContext if not previous created by following
   * steps:
   *  1. check with _instance is nullptr or not if already initialized then will
   *     return the initialized instance, if not do following step
   *  2. call static method is_registered() to check if current device is
   *     registered, if not register will goto step 3, otherwise read the device
   *     config and get saved JWT Token
   *  3. call protected static method register_client to register current device
   *     and get JWT Login Token, then write all of device information and token
   *     to device.cfg
   * If _DeviceContext is already created then will return the initialized instance
   * @param server_address
   * @param client_conf
   * @return true success initialized device context
   * @return false failed to initialized device context
   */
  [[nodiscard]] static std::shared_ptr<IDeviceContext> dev_ctx() noexcept;

 protected:
  /**
   * @brief Regist current device to remote management server
   * // TODO need to write code handle_difficult jwt renew
   */
  static void register_client();

 private:
  uint16_t _p2p_listen_port;
  std::string _server_address;
  std::string _p2p_listen_interface;
  std::filesystem::path _client_configuration_path;
  std::filesystem::path _client_certificate_path;
  std::filesystem::path _server_certifiacte_path;

  // instance ptr
  std::shared_ptr<DeviceContextImpl> _device_ctx_instance = nullptr;
};
}  // namespace P2PFileSync::ManagementAPI

#endif  // P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H