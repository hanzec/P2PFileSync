#include <curl/curl.h>
#include <openssl/pkcs12.h>

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <filesystem>
#include <string>

#include "export.h"
#include "model/model.h"

#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H

namespace P2PFileSync::Serverkit {
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
EXPORT_FUNC bool register_status();

/**
 * @brief Register client with remote management server, when register failed will
 * return {nullptr,0}
 *
 * @return RegisterClientResponse response class for exporting register result
 */
EXPORT_FUNC std::shared_ptr<RegisterClientResponse> regist_client();

/**
 * @brief internal global init function, see detail at description in c
 * interface
 *
 * @param server_url then url of management server
 * @param data_path the data and configuration folder of current daemon
 */
EXPORT_FUNC void global_init(const char* server_url, const char* data_path);

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
  CURLSH* curl_handler = nullptr;
};
}  // namespace P2PFileSync::Serverkit

#endif  // P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H