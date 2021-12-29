//
// Created by hanzech on 11/25/21.
//

#ifndef P2P_FILE_SYNC_PACKET_TASKS_H
#define P2P_FILE_SYNC_PACKET_TASKS_H
#include "protocol.pb.h"
#include "utils/ip_addr.h"

namespace P2PFileSync::Task {
// TODO need document
bool send_packet_tcp(uint16_t machine_port, SignedProtoMessage& msg,
                     const std::shared_ptr<IPAddr>& ip_addr);

}  // namespace P2PFileSync::Task

#endif  // P2P_FILE_SYNC_PACKET_TASKS_H
