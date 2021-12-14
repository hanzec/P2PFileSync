//
// Created by hanzech on 12/13/21.
//

#ifndef P2P_FILE_SYNC_IP_ADDRESS_UTILS_H
#define P2P_FILE_SYNC_IP_ADDRESS_UTILS_H
#include <string>
#include <vector>

namespace P2PFileSync::ServerKit::IPAddressUtils {
  //todo: add mode documentation
  std::vector<std::pair<std::string,std::string>> getIPAddresses();
}
#endif  // P2P_FILE_SYNC_IP_ADDRESS_UTILS_H
