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
  IPAddr(const std::string& ip_string);

  /**
  * @brief Construct a new IPAddr object
  * 
  * @param packed_data raw data which exactly 6 bytes, the data format are listed
  *        in project document.
  */
  IPAddr(const std::array<std::byte, 6>& packed_data);

  // data accessors
  [[nodiscard]]bool valid() const;
  [[nodiscard]] uint16_t port() const;

  /**
   * @brief Generated packed data for ip addr.
   * 
   * @return std::array<std::byte, 6> the packed data which will be exactly 6 bytes.
   */
  [[nodiscard]] std::array<std::byte, 6> to_bytes() const;
  
  // debug strings
  friend std::ostream &operator<<(std::ostream &os, const IPAddr &dt);

private:
  bool _valid = false;
  uint16_t _port_number = 9999;
  std::array<uint8_t, 4> _ip_addr = {255, 255, 255, 255};
};
} // namespace P2PFileSync
#endif