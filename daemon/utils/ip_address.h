/*
 * @Author: Hanze Chen
 * @Date: 2021-08-27 20:36:52
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-08-29 19:07:41
 */
#ifndef P2P_FILE_SYNC_UTILS_IP_ADDR_H
#define P2P_FILE_SYNC_UTILS_IP_ADDR_H

#include <arpa/inet.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ostream>
#include <regex>
#include <string>

#ifdef UNDER_UNIX
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#define IPV4_PATTERN \
  R"((?:25[0-5]|2[0-4]\d|[0-1]?\d{1,2})(?:\.(?:25[0-5]|2[0-4]\d|[0-1]?\d{1,2})){3}:(\d)+)"
#define IPV6_PATTERN \
  R"((([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])):(\d)+)"

namespace P2PFileSync {
class IPAddress {
 public:
  /**
   * @brief Construct a new IPAddress object
   *
   * @param ip_string string of ip address like "127.0.0.1:1224"
   */
  explicit IPAddress(const std::string &ip_string) {
    std::regex ipv4_pattern(IPV4_PATTERN);
    std::regex ipv6_pattern(IPV6_PATTERN);

    if (std::regex_match(ip_string, ipv4_pattern) ||
        std::regex_match(ip_string, ipv6_pattern)) {
      _valid = true;
      auto location = ip_string.find(':');
      _ip_address_str = ip_string.substr(0, location);
      _port_number = std::stoi(ip_string.substr(location + 1, ip_string.size()));
    } else {
      _valid = false;
      return;
    }
  }

  /**
   * @brief Copy Construct from an existing IPAddress object
   *
   * @param packed_data raw data which exactly 6 bytes, the data format are
   * listed in project document.
   */
  IPAddress(const IPAddress &src)
      : _valid(src.valid()), _port_number(src.port()), _ip_address_str(src._ip_address_str){};

  // TODO move constructor here

  /**
   * @brief Construct from a socket_in struct
   *
   * @param packed_data raw data which exactly 6 bytes, the data format are
   * listed in project document.
   */
  IPAddress(const struct sockaddr_in *socket_in, uint16_t port) {
    _port_number = port;
    if (socket_in->sin_family == AF_INET || socket_in->sin_family == AF_INET6) {
      _valid = true;
      char sip[socket_in->sin_family];
      inet_ntop(socket_in->sin_family, (void *)&socket_in->sin_addr, sip, 20);
      _ip_address_str = std::string(sip);
    } else {
      _valid = false;
    }
  }

  /**
   * @brief check if the current IPAddress object contains valid ip address
   *
   * @return true contains valid ip address
   * @return false contains invalid ip address
   */
  [[nodiscard]] bool valid() const { return _valid; }

  /**
   * @brief Get the port number of this IPAddress
   *
   * @return uint16_t the port number
   */
  [[nodiscard]] uint16_t port() const { return _port_number; }

  /**
   * @brief Get the IP Address String object
   *
   * @return std::string the string format of the ip address
   */
  [[nodiscard]] std::string ip_address() const { return _ip_address_str; }


  // debug strings
  friend std::ostream &operator<<(std::ostream &os, const IPAddress &dt) {
    os << dt._ip_address_str << ":" << dt._port_number;
    return os;
  };

  // comparator for unordered map
  bool operator==(const IPAddress &other_ip) const {
    return _port_number == other_ip._port_number &&
           _ip_address_str == other_ip._ip_address_str;
  }

 private:
  bool _valid = false;
  uint16_t _port_number = 0;
  std::string _ip_address_str;
};
}  // namespace P2PFileSync
#endif