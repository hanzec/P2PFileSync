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
//-------------------- Global Variable[NOT EXPOSED] ------------------------
// client self identifiers
static const std::string *client_id = nullptr;

// management server information
static const std::string *server_address_ = nullptr;
static const std::filesystem::path *configuration_path_ = nullptr;

// certificates

//---------------------------------------------------------------------------

class DeviceContext {
 public:
  /**
   * @brief Construct a new Device Context by following steps:
   *  1. check the class static variable [_login_token] is initalized(.empty())
   *     or not if initalized then finished else go over next steps
   *  2. call static method register_status() to check if current device is
   *     registered, if not register will goto step 3, otherwise read the client
   *     config and get saved JWT Token
   *  3. call protected static method regist_client to register current device
   *     and get JWT Login Token, then write all of client information and token 
   *     to client.cfg
   *
   * Existing Problem:
   *  1. JWT renew in not implentmented, currently server are setting expire
   *     date as 1 year after to avoid problem
   */
  EXPORT_FUNC DeviceContext();

  /**
   * @brief return the enabled status at remote management server, in this
   * function will call get_client_info() to test the response will return
   * errors or not, if downloaded reponse is contains a valid json format with
   * success and its value is true than we can say the client is enabled
   * //TODO should modified as const 
   * @return true this client is enabled at management server
   * @return false this client is not enabled at management server
   */
  [[nodiscard]] EXPORT_FUNC bool is_enabled();

  /**
   * @brief Request PKCS12 certificate of current client, this function will
   * create an local cache of loaded certificate in memory, once the certificate
   * is loaded from disk, then every time copy this pointer will return cached
   * result.
   * @note the user DO-NOT free this returned pointer
   * @return PKCS12* loaded pointer of OPENSSL pkcs12 format certificate
   */
  [[nodiscard]] EXPORT_FUNC const PKCS12* get_certificate();

  /**
   * @brief Returned the neighbor peer list from management server
   *
   * @return std::unordered_map<std::string,std::string> list of peer cancidate's ip id and its ip address
   */
  [[nodiscard]] EXPORT_FUNC std::unordered_map<std::string,std::string> get_peer_list();

  /**
   * @brief Returned current client register information from remote management
   * server
   *
   * @return ClientInfoResponse the response struct contains information
   */
  [[nodiscard]] EXPORT_FUNC std::unique_ptr<ClientInfoResponse> get_client_info();

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
  [[nodiscard]] EXPORT_FUNC static bool register_status();

  /**
   * @brief Request PKCS12 certificate of signed servert, this function will
   * create an local cache of loaded certificate in memory, once the certificate
   * is loaded from disk, then every time copy this pointer will return cached
   * result.
   * @note the user DO-NOT free this returned pointer
   * @return PKCS7* loaded pointer of OPENSSL pkcs7 format certificate
   */
  [[nodiscard]] EXPORT_FUNC static const PKCS7* get_server_sign_certificate();

 protected:
  /**
   * @brief Regist current device to remote management server
   * // TODO need to write code handle jwt renew
   * @return std::string will return the JWT Token authorized by management
   * server after register
   */
  EXPORT_FUNC static std::string regist_client();

 private:
  /**
   * Certificates
   */
  inline static PKCS12 *_client_cert = nullptr;
  inline static PKCS7 *_client_sign_cert = nullptr;

  inline static std::string _login_token = "";
};

/**
 * @brief The cpp class for server session per connection
 *
 */
class ManagementContext {
 public:
  EXPORT_FUNC ManagementContext(bool strict_security = true);

  /**
   * @brief Destroy the Server Connection object
   *  This function will destroy following things:
   *    - Free the
   */
  EXPORT_FUNC ~ManagementContext();

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
 * @brief internal global init function, see detail at description in c
 * interface
 *
 * @param client_config the client config
 */
EXPORT_FUNC void global_init(const Config &client_config);
}  // namespace P2PFileSync::Serverkit

#endif  // P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H