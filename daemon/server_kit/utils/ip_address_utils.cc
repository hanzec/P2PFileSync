//
// Created by hanzech on 12/13/21.
//

#include "ip_address_utils.h"

#include <arpa/inet.h>
#include <glog/logging.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <unistd.h>

#include <cerrno>

namespace P2PFileSync::ServerKit::IPAddressUtils {

std::vector<std::string> getIPAddresses() {
  struct ifaddrs *ifList = nullptr;
  char address_buf[INET_ADDRSTRLEN];
  std::vector<std::string> addresses;
  if (getifaddrs(&ifList) >= 0) {
    for (struct ifaddrs *ifa = ifList; ifa != nullptr; ifa = ifa->ifa_next) {
      auto interface_address = (struct sockaddr_in *)ifa->ifa_addr;
      if (inet_ntop(
              interface_address->sin_family, &interface_address->sin_addr, address_buf,
              interface_address->sin_family == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN)) {
        addresses.emplace_back(address_buf);
        VLOG(3) << "find the local IP address [" << address_buf << "] for interface ["
                << ifa->ifa_name << "]";
      } else {
        LOG(INFO) << "skip interface [" << ifa->ifa_name << "]: " << strerror(errno);
      }
    }
//    if (ifList != nullptr)
//      freeifaddrs(ifList);
  } else {
    LOG(ERROR) << "failed on gethostname:" << strerror(errno);
  }
  return addresses;
}

}  // namespace P2PFileSync::ServerKit::IPAddressUtils