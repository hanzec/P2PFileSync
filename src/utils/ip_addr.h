#ifndef P2P_FILE_SYNC_UTILS_IP_ADDR_H
#define P2P_FILE_SYNC_UTILS_IP_ADDR_H

#include <regex>
#include <array>
#include <cstdint>
#include <utility>

namespace P2PFileSync {
    class IPAddr {
      public:
        IPAddr(const std::string ip_string){
            std::regex ip_pattern("((?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(?:(?<!.)\b|.)){4}:")
        }
        std::string ip_str() const {

        }

        friend std::ostream& operator<<(std::ostream& os, const IPAddr& dt){
            os << dt._ip_addr[0] << "." 
               << dt._ip_addr[0] << "." 
               << dt._ip_addr[0] << "." 
               << dt._ip_addr[0] << ":"  << dt._port_number;
            return os;
        }
      private:
        const bool valid;
        const uint16_t _port_number;
        const std::array<uint8_t, 4> _ip_addr;
   };
} // P2PFileSync
#endif