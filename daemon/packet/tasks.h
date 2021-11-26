//
// Created by hanzech on 11/25/21.
//

#ifndef P2P_FILE_SYNC_TASKS_H
#define P2P_FILE_SYNC_TASKS_H
#include "protocol.pb.h"
#include "utils/ip_addr.h"

namespace P2PFileSync {
static bool send_packet(const SignedProtoMessage& msg, const IPAddr& ip_addr);
}

#endif  // P2P_FILE_SYNC_TASKS_H
