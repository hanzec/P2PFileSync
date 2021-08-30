/*
 * @Author: Hanze CHen 
 * @Date: 2021-08-29 21:12:46 
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-08-29 21:22:11
 */
#ifndef P2P_FILE_SYNC_UTILS_CONFIG_READER_H
#define P2P_FILE_SYNC_UTILS_CONFIG_READER_H

#include <memory>
#include "common.h"

namespace P2PFileSync {
  [[nodiscard]] bool generate_default_config(const std::string& config_file);
  [[nodiscard]] std::shared_ptr<Config> parse_comfig_file(const std::string& config_file);
} // P2PFileSync

#endif