#include "message.h"

namespace P2PFileSync {
bool Message::Header::set_src_ip(const IPAddr& ip_addr) {
  _source_ip = IPAddr(ip_addr);
  return true;
};
bool Message::Header::set_src_ip(const std::string& ip_addr) {
  _source_ip = IPAddr(ip_addr);
  return _source_ip.valid();
};
bool Message::Header::set_des_ip(const IPAddr& ip_addr) {
  _destination_ip = IPAddr(ip_addr);
  return true;
};

bool Message::Header::set_des_ip(const std::string& ip_addr) {
  _destination_ip = IPAddr(ip_addr);
  return _destination_ip.valid();
}

// debug strings
std::ostream& operator<<(std::ostream& os, const Message::Header& header) {
  os << "packet from [" << header._source_ip << "] to ["
     << header._destination_ip << "] with size of [" << header._payload_size
     << "[";
  return os;
}

}  // namespace P2PFileSync