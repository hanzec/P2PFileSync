/*
 * @Author: Hanze CHen 
 * @Date: 2021-10-23 16:57:10 
 * @Last Modified by:   Hanze Chen 
 * @Last Modified time: 2021-10-23 16:57:10 
 */
#include "ip_addr.h"

namespace P2PFileSync {
IPAddr::IPAddr(const std::string &ip_string) {
  std::regex ip_pattern(
      R"((?:25[0-5]|2[0-4]\d|[0-1]?\d{1,2})(?:\.(?:25[0-5]|2[0-4]\d|[0-1]?\d{1,2})){3}:(\d)+)");

  if (std::regex_match(ip_string, ip_pattern)) {
    _valid = true;
    auto location = ip_string.find(':');
    std::string ip = ip_string.substr(0, location);
    _port_number = std::stoi(ip_string.substr(location + 1, ip_string.size()));
#ifdef UNDER_UNIX
    {
      in_addr_t ip_addr = inet_addr(ip.c_str());
      memcpy(_ip_addr.data(), &ip_addr, sizeof(in_addr_t));
    }
#endif
  }
};

IPAddr::IPAddr(const std::array<std::byte, 6> &packed_data) {
  memcpy(_ip_addr.data(), packed_data.data(), sizeof(_ip_addr));
  memcpy(&_port_number, packed_data.data() + sizeof(_ip_addr), sizeof(_port_number));
};

// data accessors
[[nodiscard]] bool IPAddr::valid() const { return _valid; }
[[nodiscard]] uint16_t IPAddr::port() const { return _port_number; }

// packed functions
[[nodiscard]] std::array<std::byte, 6> IPAddr::to_bytes() const {
  std::array<std::byte, 6> result;
  memcpy(result.data(), _ip_addr.data(), sizeof(_ip_addr));
  memcpy(result.data() + sizeof(_ip_addr), &_port_number, sizeof(_port_number));
  return result;
};

// debug strings
std::ostream &operator<<(std::ostream &os, const IPAddr &dt) {
  os << static_cast<unsigned int>(dt._ip_addr[0]) << "."
     << static_cast<unsigned int>(dt._ip_addr[1]) << "."
     << static_cast<unsigned int>(dt._ip_addr[2]) << "."
     << static_cast<unsigned int>(dt._ip_addr[3]) << ":" << dt._port_number;
  return os;
};
}  // namespace P2PFileSync