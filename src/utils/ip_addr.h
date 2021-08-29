/*
 * @Author: Hanze Chen 
 * @Date: 2021-08-27 20:36:52 
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-08-29 15:53:59
 */
#ifndef P2P_FILE_SYNC_UTILS_IP_ADDR_H
#define P2P_FILE_SYNC_UTILS_IP_ADDR_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
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
  IPAddr(const std::string& ip_string) {
    std::regex ip_pattern(
        "(?:25[0-5]|2[0-4]\\d|[0-1]?\\d{1,2})(?:\\.(?:25[0-5]|2[0-4]\\d|[0-1]?\\d{1,2})){3}:(\\d)+");

    if (std::regex_match(ip_string, ip_pattern)) {
      _valid = true;
      auto location = ip_string.find(":");
      std::string ip = ip_string.substr(0, location);
      _port_number = std::stoi(ip_string.substr(location + 1, ip_string.size()));
#ifdef UNDER_UNIX
      {
        in_addr_t ip_addr = inet_addr(ip.c_str());
        memcpy(_ip_addr.data(), &ip_addr, sizeof(in_addr_t));
      }
#endif
    }
  }

  /**
  * @brief Construct a new IPAddr object
  * 
  * @param packed_data raw data which exactly 6 bytes, the data format are listed
  *        in project document.
  */
  IPAddr(const std::array<std::byte, 6>& packed_data){
    memcpy(_ip_addr.data(), packed_data.data(), sizeof(_ip_addr));
    memcpy(&_port_number, packed_data.data() + sizeof(_ip_addr), sizeof(_port_number));
  }

  // data accessors
  [[nodiscard]] bool valid() const { return _valid;}
  [[nodiscard]] uint16_t port() const {return _port_number;}

  /**
   * @brief Generated packed data for ip addr.
   * 
   * @return std::array<std::byte, 6> the packed data which will be exactly 6 bytes.
   */
  [[nodiscard]] std::array<std::byte, 6> to_bytes() const {
    std::array<std::byte, 6> result;
    memcpy(result.data(), _ip_addr.data(), sizeof(_ip_addr));
    memcpy(result.data() + sizeof(_ip_addr), &_port_number, sizeof(_port_number));
    return result;
  }

  // debug strings
  friend std::ostream &operator<<(std::ostream &os, const IPAddr &dt) {
    os << static_cast<unsigned int>(dt._ip_addr[0]) << "." 
       << static_cast<unsigned int>(dt._ip_addr[1]) << "."
       << static_cast<unsigned int>(dt._ip_addr[2]) << "." 
       << static_cast<unsigned int>(dt._ip_addr[3]) << ":" << dt._port_number;
    return os;
  }

private:
  bool _valid = false;
  uint16_t _port_number = 9999;
  std::array<uint8_t, 4> _ip_addr = {255, 255, 255, 255};
};
} // namespace P2PFileSync
#endif