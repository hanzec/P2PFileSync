//
// Created by chen_ on 2021/12/31.
//

#ifndef P2P_FILE_SYNC_MANAGEMENT_API_H
#define P2P_FILE_SYNC_MANAGEMENT_API_H
#include "configuration.h"
#include "management_api_export.h"
#include "model/IDeviceInfoResponse.h"
#include "model/IPeerListResponse.h"
#include "model/IUserDetailResponse.h"
#include "utils/macro.h"

namespace P2PFileSync::ManagementAPI {
class MANAGEMENT_API_EXPORT IUserContext {
 public:
  /**
   * @brief Not provide, use ManagementContext::usr_ctx() instead
   */
  MANAGEMENT_API_EXPORT IUserContext() = default;

  /**
   * @brief Destroy the Server Connection object
   *  This function will destroy following things:
   *    - Free the _share_handle
   */
  MANAGEMENT_API_EXPORT virtual ~IUserContext() = default;

  /**
   * @brief Logout the user from the server in current login session
   *
   * @note this method will do nothing if the user is not logged in
   */
  MANAGEMENT_API_EXPORT virtual void logout() = 0;

  /**
   * @brief LOCALLY check the user login status, will NOT send any request to management server
   * @return true if the user is logged in
   * @return false if the user is not logged in
   */
  [[nodiscard]] MANAGEMENT_API_EXPORT virtual bool is_logged_in() const = 0;

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
  [[nodiscard]] MANAGEMENT_API_EXPORT virtual std::pair<bool, std::string> login(
      const std::string& email, const std::string& password) = 0;

  /**
   * @brief Get the user detail information from the management server
   *
   * @note this will not cached, so everytime this function is called, it will send a request
   *       to the management server
   * @return UserDetailResponseImpl the user detail information
   */
  [[nodiscard]] MANAGEMENT_API_EXPORT virtual std::unique_ptr<IUserDetailResponse>
  user_detail() const = 0;
};

/**
 * @brief User to manage the peer device related requests, this class is a singleton class.
 */
class MANAGEMENT_API_EXPORT IDeviceContext : std::enable_shared_from_this<IDeviceContext> {
 public:
  /**
   * @brief make sure this object only construct and get by init_dev_ctx()
   */
  MANAGEMENT_API_NO_EXPORT IDeviceContext() = default;
  MANAGEMENT_API_NO_EXPORT IDeviceContext(IDeviceContext&) = delete;
  MANAGEMENT_API_NO_EXPORT IDeviceContext(IDeviceContext&&) = delete;
  MANAGEMENT_API_NO_EXPORT IDeviceContext& operator=(IDeviceContext&) = delete;
  MANAGEMENT_API_NO_EXPORT IDeviceContext& operator=(IDeviceContext&&) = delete;

  MANAGEMENT_API_NO_EXPORT virtual ~IDeviceContext() = default;

  /**
   * @brief return the enabled status at remote management server, in this
   *        function will call device_info() to test the response will return
   *        errors or not, if downloaded reponse is contains a valid json format with
   *        success and its value is true than we can say the device is enabled
   * @return true this device is enabled at management server
   * @return false this device is not enabled at management server
   */
  [[nodiscard]] MANAGEMENT_API_EXPORT virtual bool is_enabled() const = 0;

  /**
   * @brief Get the id of current peer
   * @note this will return the local cache of the id loaded from the Device configuration
   *       file, since the device id will never change after the device is registered
   * @return const std::string& the device id as string of the object
   */
  [[nodiscard]] MANAGEMENT_API_EXPORT virtual const std::string& device_id() const = 0;

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
  [[nodiscard]] MANAGEMENT_API_EXPORT virtual std::filesystem::path client_certificate()
      const = 0;

  /**
   * @brief Returned the neighbor peer list from management server
   * @note this function will always send request to server each time.
   * @return std::unordered_map<std::string,std::string> list of peer
   * cancidate's ip id and its ip address
   */
  [[nodiscard]] MANAGEMENT_API_EXPORT virtual std::unique_ptr<IPeerListResponse> peer_list()
      const = 0;

  /**
   * @brief Returned current device register information from remote management
   *        server
   * @note this method will always send request to server each time.
   * @return DeviceInfoResponseImpl the response struct contains information
   */
  [[nodiscard]] MANAGEMENT_API_EXPORT virtual std::unique_ptr<IDeviceInfoResponse>
  device_info() const = 0;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Construct a new Server Context object.
 *
 * @note this method will be only called by init()
 * @param config config of the current program
 * @param this_is_private protect the caller from calling this constructor
 */
[[maybe_unused]] MANAGEMENT_API_EXPORT STD_CALL_CONV void init(const Config& config) noexcept;

/**
 * @brief Get the version of Management API lib without any extra information
 *
 * @note this should be generate when compile time
 * @return std::string version string which is in [major.minor.patch]
 * format
 */
[[maybe_unused]] MANAGEMENT_API_EXPORT constexpr STD_CALL_CONV std::string_view version();

/**
 * @brief Get the Complete version of Management API lib including git hash and build time
 *
 * @note this should be generate when compile time
 * @return std::string version string which is in [major.minor.patch-git hash(Build time)]
 * format
 */
[[maybe_unused]] MANAGEMENT_API_EXPORT constexpr STD_CALL_CONV std::string_view full_version();

/**
 * @brief Get an new instance of _UserContext
 *
 * @return _UserContext the instance of _UserContext
 */
[[maybe_unused]] MANAGEMENT_API_EXPORT STD_CALL_CONV std::unique_ptr<IUserContext>
usr_ctx() noexcept;

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
[[maybe_unused]] MANAGEMENT_API_EXPORT STD_CALL_CONV std::shared_ptr<IDeviceContext>
dev_ctx() noexcept;
};
}  // namespace P2PFileSync::ManagementAPI
#endif  // P2P_FILE_SYNC_MANAGEMENT_API_H
