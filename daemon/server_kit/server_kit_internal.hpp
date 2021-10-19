#include <algorithm>
#include <string>
#include <atomic>
#include <filesystem>
#include "../utils/status.h"
#include <curl/curl.h>
#include <openssl/pkcs12.h>

#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_INTERNAL_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_KI_INTERNALT_H

namespace P2PFileSync::Server_kit {

  /** 
   * Global variables will init at the call of P2PFileSync_SK_global_init()
   */
  // management server information
  static const std::string * server_address_;
  static const std::filesystem::path * configuration_path_;

  // certificates
  static const PKCS7 * client_sign_certificate_;

  /**
   * @brief The cpp class for server session per connection
   * 
   */
  class ServerConnection{
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
    private:
      bool strict_security_ = true;
      CURLSH * curl_handler = nullptr;
  };
} // namespace P2PFileSync

#endif // P2P_FILE_SYNC_SERVER_KIT_SERVER_KI_INTERNALT_H