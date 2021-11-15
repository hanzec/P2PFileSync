#include <curl/curl.h>
#include <openssl/pkcs12.h>

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

#include "common.h"
#include "export.h"
#include "model/response/response.h"

#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H

namespace P2PFileSync::Serverkit {
/**
 * @brief The cpp class for server session per connection
 *
 */
class UserContext {
 public:
  EXPORT_FUNC explicit UserContext(bool strict_security = true);

  /**
   * @brief Destroy the Server Connection object
   *  This function will destroy following things:
   *    - Free the
   */
  EXPORT_FUNC ~UserContext();

  // /**
  //  * @brief Register new Client
  //  *
  //  */
  // Status register_client();

  // // TODO finish dnssec
  // /**
  //  * @brief Enable strict security
  //  *  Enable strict security which means enable following feature:
  //  *    - DNSSEC support [todo]
  //  *    - Coneecting server only HTTPS ONLY
  //  */
  // void enable_strict_security();

  // /**
  //  * @brief Disbale strict security
  //  *  Disbale strict security which means disable following feature:
  //  *    - DNSSEC support [todo]
  //  *    - Coneecting server over HTTPS ONLY
  //  */
  // void disbale_strict_security();

 private:
  bool strict_security_ = true;
  CURLSH *curl_handler = nullptr;
};

/**
 * @brief User to manage the peer device related requests
 */
class DeviceContext {
 public:
  /**
   * @brief make sure this object only construct and get by get_one()
   */
  DeviceContext() = delete;

  /**
   * @brief get instance of the DeviceContext
   *  1. check with _instance is nullptr or not if already initialized then will
   *     return the initialized instance, if not do following step
   *  2. call static method is_registered() to check if current device is
   *     registered, if not register will goto step 3, otherwise read the client
   *     config and get saved JWT Token
   *  3. call protected static method register_client to register current device
   *     and get JWT Login Token, then write all of client information and token
   *     to client.cfg
   * @return
   */
  [[nodiscard]] EXPORT_FUNC static std::shared_ptr<DeviceContext> get_one();

  /**
   * @brief return the enabled status at remote management server, in this
   * function will call client_info() to test the response will return
   * errors or not, if downloaded reponse is contains a valid json format with
   * success and its value is true than we can say the client is enabled
   * //TODO should modified as const
   * @return true this client is enabled at management server
   * @return false this client is not enabled at management server
   */
  [[nodiscard]] EXPORT_FUNC bool is_enabled() const;

  /**
   * @brief Check if client is registered or not by following rules:
   *  1. check if there exist a file under config folder called client.cfg
   *  2. check if there exist a file under config folder called client.pem
   *  3. //TODO check with server to validate the validation of the client
   * certificate
   *  4. //TODO verify the client machine id is matching the cfg file and
   * certificate
   * @return true the client is registed with server
   * @return false ther client is not registed with server
   */
  [[nodiscard]] EXPORT_FUNC static bool is_registered();

  /**
   * @brief Get the id of current peer
   *
   * @return const std::string& the client id as string of the object
   */
  [[nodiscard]] EXPORT_FUNC const std::string& client_id() const;

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
   * @return ClientInfoResponse the response struct contains information
   */
  [[nodiscard]] EXPORT_FUNC std::unique_ptr<ClientInfoResponse> client_info() const;

  /**
   * @brief Returned the neighbor peer list from management server
   *
   * @return std::unordered_map<std::string,std::string> list of peer
   * cancidate's ip id and its ip address
   */
  [[nodiscard]] EXPORT_FUNC std::map<std::string, std::string> peer_list() const;

 protected:
  /**
   * @brief Construct a new Device Context by following steps:
   *
   * Existing Problem:
   *  1. JWT renew in not implentmented, currently server are setting expire
   *     date as 1 year after to avoid problem
   */
  EXPORT_FUNC DeviceContext(std::string client_id, std::string client_token);

  /**
   * @brief Regist current device to remote management server
   * // TODO need to write code handle jwt renew
   * @return std::pair<std::string,std::string> will return the JWT Token
   * authorized by management server after register and the register id by
   * formate of <JWT Token, Peer ID>
   */
  EXPORT_FUNC static std::pair<std::string, std::string> register_client();

 private:
  // instance ptr
  inline static std::shared_ptr<DeviceContext> _instance = nullptr;

  /**
   * Client information
   */
  const std::string _client_id;
  const std::string _login_token;
};

/**
 * @brief internal global init function, see detail at description in c
 * interface
 *
 * @param client_config the client config
 */
EXPORT_FUNC void global_init(const std::shared_ptr<Config>& client_config);
}  // namespace P2PFileSync::Serverkit

#endif  // P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H