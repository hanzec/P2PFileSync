#include <curl/curl.h>
#include <openssl/pkcs12.h>

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <filesystem>
#include <string>

#include "model/model.hpp"

#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_INTERNAL_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_KI_INTERNALT_H

namespace P2PFileSync::Server_kit {

/**
 * Global variables will init at the call of P2PFileSync_SK_global_init()
 */
// client self identifiers
static const std::string* client_id = nullptr;

// management server information
static const std::string* server_address_ = nullptr;
static const std::filesystem::path* configuration_path_ = nullptr;

// certificates
static const PKCS12* client_certificate_ = nullptr;
static const PKCS7* client_sign_certificate_ = nullptr;

/**
 * @brief The cpp class for server session per connection
 *
 */
class ServerConnection {
 public:
  ServerConnection(bool strict_security = true);

  /**
   * @brief Destroy the Server Connection object
   *  This function will destroy following things:
   *    - Free the
   */
  ~ServerConnection();

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

  /**
   * @brief internal client register function, see detail at description in c
   * interface
   *
   * @return true the client is registed with server
   * @return false ther client is not registed with server
   */
  static bool is_registered();

  /**
   * @brief Internal function for register client, when register failed will
   * return {nullptr,0}, see detail at description in c interface
   *
   * @return std::pair<std::string,int> <activation url, server reg code>
   */
  static std::pair<std::string, int> regist_client();

  /**
   * @brief internal global init function, see detail at description in c
   * interface
   *
   * @param server_url then url of management server
   * @param data_path the data and configuration folder of current daemon
   */
  static void global_init(const char* server_url, const char* data_path);

 private:
  bool strict_security_ = true;
  CURLSH* curl_handler = nullptr;
};
}  // namespace P2PFileSync::Server_kit

#endif  // P2P_FILE_SYNC_SERVER_KIT_SERVER_KI_INTERNALT_H