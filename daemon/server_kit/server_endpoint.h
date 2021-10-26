/*
 * @Author: Hanze CHen 
 * @Date: 2021-10-03 00:16:32 
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-10-03 00:17:15
 */
#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_ENDPOINT_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_ENDPOINT_H
#include <string>

namespace P2PFileSync::Server_kit {

  const static std::string CLIENT_CFG_NAME = "client.cfg";

  const static std::string CLIENT_CERT_NAME = "client.p12";

  const static std::string SERVER_CERT_NAME = "client_sign_root.crt";

  const static std::string SERVER_CERT_ENDPOINT_V1 = "/client_sign_root.crt";
  
  const static std::string SERVER_REGISTER_ENDPOINT_V1 = "/api/v1/register_client";


} // namespace P2PFileSync::P2PFileSync::Server_kit
#endif //P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H