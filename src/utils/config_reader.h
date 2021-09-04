/*
 * @Author: Hanze CHen 
 * @Date: 2021-08-29 21:12:46 
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-08-30 22:36:56
 */
#ifndef P2P_FILE_SYNC_UTILS_CONFIG_READER_H
#define P2P_FILE_SYNC_UTILS_CONFIG_READER_H

#include <memory>
#include "common.h"

namespace P2PFileSync {
  /**
   * @brief this function will generate default app config
   * 
   * @param config_file the generated file location
   * @return true if generate success
   * @return false if generate failed
   */
  [[nodiscard]] bool generate_default_config(const std::string& config_file);

  /**
   * @brief parser from yaml like config file to Config class
   * 
   * @param config_file location of yaml like config file
   * @return std::shared_ptr<Config> the parsed configured object
   */
  [[nodiscard]] std::shared_ptr<Config> parse_comfig_file(const std::string& config_file);
} // P2PFileSync

#endif