#ifndef P2P_FILE_SYNC_MANAGE_H
#define P2P_FILE_SYNC_MANAGE_H
#include <sys/socket.h>

#include <array>
#include <string>

#include "daemon_state.h"
#include "utils/ip_addr.h"

namespace  P2PFileSync{
/**
 * @brief Start manage interface for P2PFileSync with socket address
 *
 * @param ip_address the address and port need to listen
 */
void manage_interface_thread(const std::string& socket, int listen_type);
} // P2PFileSync
#endif
