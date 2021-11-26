//
// Created by hanzech on 11/25/21.
//

#include "tasks.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <glog/logging.h>

namespace P2PFileSync::Task {
bool send_packet_tcp(SignedProtoMessage& msg, const std::shared_ptr<IPAddr>& ip_addr) {
  int server_sock = 0;
  struct sockaddr_in serv_addr{};
  if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    LOG(ERROR) << "failed to open the socket!";
    return false;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(ip_addr->port());
  // Converting IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, ip_addr->ip().c_str(), &serv_addr.sin_addr) <= 0) {
    LOG(ERROR) << "Invalid address ! This IP Address is not supported !";
    return false;
  }

  if (connect(server_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    LOG(ERROR) << "Connection Failed : Can't establish a connection over this socket !";
    return false;
  }

  msg.set_ttl(msg.ttl() - 1); // decrease 1 to ttl

  size_t proto_size = msg.ByteSizeLong();
  auto raw_proto = malloc(proto_size);
  msg.SerializeToArray(raw_proto,proto_size);
  send(server_sock, raw_proto, proto_size, 0);
  free(raw_proto);
  return true;
}
}  // namespace P2PFileSync