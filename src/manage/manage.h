#ifndef P2P_FILE_SYNC_MANAGE_MANAGE_H
#define P2P_FILE_SYNC_MANAGE_MANAGE_H
#include "utils/ip_addr.h"
extern "C" {
  void P2PFileSync_start_manage_thread(P2PFileSync::IPAddr listen_socket);
};
#endif
