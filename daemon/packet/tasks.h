//
// Created by hanzech on 11/25/21.
//

#ifndef P2P_FILE_SYNC_TASKS_H
#define P2P_FILE_SYNC_TASKS_H
#include "protocol.pb.h"
#include "utils/ip_addr.h"

namespace P2PFileSync::Task {
//TODO need document
bool send_packet_tcp(SignedProtoMessage& msg, const std::shared_ptr<IPAddr>& ip_addr);
}

#endif  // P2P_FILE_SYNC_TASKS_H