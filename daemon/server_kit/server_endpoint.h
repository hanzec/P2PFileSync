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

namespace P2PFileSync::ServerKit {
  /**
   * Authenticate related API
   */
  const constexpr static std::string_view SERVER_PASSWORD_LOGIN_V1 = "/api/v1/login";
  const constexpr static std::string_view SERVER_PASSWORD_LOGOUT_V1 = "/logout";
  const constexpr static std::string_view SERVER_REGISTER_ENDPOINT_V1 = "/api/v1/register_client";

  /**
   * User management related API
   */
  const constexpr static std::string_view SERVER_USER_DETAIL_V1 = "/api/v1/user";


  /**
   * Client management related API
   */
  const constexpr static std::string_view GET_CLIENT_INFO_ENDPOINT_V1 = "/api/v1/client";
  const constexpr static std::string_view GET_CLIENT_PEER_INFO_ENDPOINT_V1 = "/api/v1/client/peer";
  const constexpr static std::string_view GET_CLIENT_GROUP_INFO_ENDPOINT_V1 = "/api/v1/client/group";
  const constexpr static std::string_view GET_CLIENT_CERTIFICATE_ENDPOINT_V1 = "/api/v1/client/certificate";
} // namespace P2PFileSync::P2PFileSync::ServerKit
#endif //P2P_FILE_SYNC_SERVER_KIT_SERVER_CONST_H