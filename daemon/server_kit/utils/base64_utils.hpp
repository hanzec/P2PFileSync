#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_BASE64_UTILS_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_BASE64_UTILS_H

#include <array>

namespace P2PFileSync::Server_kit::Base64 {

  /**
   * @brief return a new string object contains the Base64 encoded string
   * 
   * @param text the string wants to do base64 encoding
   * @return std::string the encoded string
   */
  std::string encode(const std::string& text);

  /**
   * @brief returned a new string object contins the decoded base 64 string
   * 
   * @param text the base 64 encoded string
   * @return std::string the decoded retsult
   */
  std::string decode(const std::string& text);

}  // namespace P2PFileSync::Base64

#endif  // P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_BASE64_UTILS_H