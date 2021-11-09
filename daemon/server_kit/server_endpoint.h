/*
 * @Author: Hanze CHen 
 * @Date: 2021-10-03 00:16:32 
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-10-03 00:17:15
 */
#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_CONST_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_CONST_H
#include <string>
#include <string_view>

namespace P2PFileSync::Serverkit {
  /**
   * Server-kit Configuration file Name
   */
  const static std::string CLIENT_CONFIGURE_FILE_NAME = "client.cfg";
  const static std::string CLIENT_CERTIFICATE_FILE_NAME = "client.p12";

  /**
   * Management server related API
   */
  const static std::string SERVER_CERT_ENDPOINT_V1 = "/client_sign_root.crt";
  
  /**
   * Authenticate related API
   */
  const static std::string SERVER_REGISTER_ENDPOINT_V1 = "/api/v1/register_client";

  /**
   * Client management related API
   */
  const static std::string GET_CLIENT_INFO_ENDPOINT_V1 = "/api/v1/client";
  const static std::string GET_CLIENT_PEER_INFO_ENDPOINT_V1 = "/api/v1/client/peer";
  const static std::string GET_CLIENT_GROUP_INFO_ENDPOINT_V1 = "/api/v1/client/group";
  const static std::string GET_CLIENT_CERTIFICATE_ENDPOINT_V1 = "/api/v1/client/certificate";

} // namespace P2PFileSync::P2PFileSync::Serverkit
#endif //P2P_FILE_SYNC_SERVER_KIT_SERVER_CONST_H