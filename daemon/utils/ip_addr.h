/*
 * @Author: Hanze Chen
 * @Date: 2021-08-27 20:36:52
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-08-29 19:07:41
 */
#ifndef P2P_FILE_SYNC_UTILS_IP_ADDR_H
#define P2P_FILE_SYNC_UTILS_IP_ADDR_H

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

namespace P2PFileSync {
class IPAddr {
 public:
  /**
   * @brief Construct a new IPAddr object
   *
   * @param ip_string string of ip address like "127.0.0.1:1224"
   */
  explicit IPAddr(const std::string &ip_string) {
    std::regex ip_pattern(
        R"((?:25[0-5]|2[0-4]\d|[0-1]?\d{1,2})(?:\.(?:25[0-5]|2[0-4]\d|[0-1]?\d{1,2})){3}:(\d)+)");

    if (std::regex_match(ip_string, ip_pattern)) {
      _valid = true;
      auto location = ip_string.find(':');
      _ip_addr_str = ip_string.substr(0, location);
      _port_number = std::stoi(ip_string.substr(location + 1, ip_string.size()));
#ifdef UNDER_UNIX
      {
        in_addr_t ip_addr = inet_addr(_ip_addr_str.c_str());
        memcpy(_ip_addr.data(), &ip_addr, sizeof(in_addr_t));
      }
#endif
    }
  }

  /**
   * @brief Copy Construct from an existing IPAddr object
   *
   * @param packed_data raw data which exactly 6 bytes, the data format are
   * listed in project document.
   */
  IPAddr(const IPAddr &src)
      : _valid(src.valid()),
        _port_number(src.port()),
        _ip_addr_str(src._ip_addr_str),
        _ip_addr(src._ip_addr) {};

  // TODO move constructor here

  /**
   * @brief Construct a new IPAddr object
   *
   * @param packed_data raw data which exactly 6 bytes, the data format are
   * listed in project document.
   */
  explicit IPAddr(const std::array<std::byte, 6> &packed_data) {
    memcpy(_ip_addr.data(), packed_data.data(), sizeof(_ip_addr));
    memcpy(&_port_number, packed_data.data() + sizeof(_ip_addr), sizeof(_port_number));
  }

  /**
   * @brief Construct from a socket_in struct
   *
   * @param packed_data raw data which exactly 6 bytes, the data format are
   * listed in project document.
   */
  IPAddr(const struct sockaddr_in *socket_in, uint16_t port) {
    char sip[20];
    _port_number = port;
    inet_ntop(socket_in->sin_family, (void *)&socket_in->sin_addr, sip, 20);
    _ip_addr_str = std::string(sip);
    memcpy(_ip_addr.data(), &socket_in->sin_addr, sizeof(in_addr_t));
  }

  /**
   * @brief Generated packed data for ip addr.
   *
   * @return std::array<std::byte, 6> the packed data which will be exactly 6
   * bytes.
   */
  [[nodiscard]] std::array<std::byte, 6> to_bytes() const {
    std::array<std::byte, 6> result{};
    memcpy(result.data(), _ip_addr.data(), sizeof(_ip_addr));
    memcpy(result.data() + sizeof(_ip_addr), &_port_number, sizeof(_port_number));
    return result;
  };

  /**
   * @brief Generated packed data for ip addr.
   *
   * @return std::array<std::byte, 6> the packed data which will be exactly 6
   * bytes.
   */
  [[nodiscard]] std::string ip_address() const { return _ip_addr_str; }

  // data accessors
  [[nodiscard]] bool valid() const { return _valid; }
  [[nodiscard]] uint16_t port() const { return _port_number; }
  [[nodiscard]] std::string ip() const { return _ip_addr_str; }

  // debug strings
  friend std::ostream &operator<<(std::ostream &os, const IPAddr &dt) {
    os << static_cast<unsigned int>(dt._ip_addr[0]) << "."
       << static_cast<unsigned int>(dt._ip_addr[1]) << "."
       << static_cast<unsigned int>(dt._ip_addr[2]) << "."
       << static_cast<unsigned int>(dt._ip_addr[3]) << ":" << dt._port_number;
    return os;
  };

  // comparator for unordered map
  bool operator==(const IPAddr &other_ip) const {
    return _port_number == other_ip._port_number && _ip_addr == other_ip._ip_addr;
  }

 private:
  bool _valid = false;
  uint16_t _port_number = 0;
  std::string _ip_addr_str = "255.255.255.255";
  std::array<uint8_t, 4> _ip_addr = {255, 255, 255, 255};
};
}  // namespace P2PFileSync
#endif