/*
 * @Author: Hanze Chen 
 * @Date: 2021-08-27 20:37:04 
 * @Last Modified by:   Hanze Chen 
 * @Last Modified time: 2021-08-27 20:37:04 
 */
#ifndef P2P_FILE_SYNC_SYNC_H
#define P2P_FILE_SYNC_SYNC_H

#include "utils/status.h"
#include "utils/ip_addr.h"

namespace P2PFileSync {
    static Status start_app(std::vector<IPAddr> known_host, std::string mount_point);
} // namespace P2PFileSync
#endif