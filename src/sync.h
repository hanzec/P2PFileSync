/*
 * @Author: Hanze Chen
 * @Date: 2021-08-27 20:37:04
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-08-29 22:22:02
 */
#ifndef P2P_FILE_SYNC_SYNC_H
#define P2P_FILE_SYNC_SYNC_H

#include "common.h"
#include "utils/ip_addr.h"
#include "utils/status.h"

namespace P2PFileSync {
/**
 * @brief Entry point of P2PFileSync app
 *
 * @param known_host list of know host giving when program started
 * @param mount_point mounting point of file system
 * @param sync_data_dir program data folder
 * @return Status The status when app is stoped
 */
Status start_app(std::vector<IPAddr> known_host,
                 const std::shared_ptr<Config> config);

}  // namespace P2PFileSync
#endif