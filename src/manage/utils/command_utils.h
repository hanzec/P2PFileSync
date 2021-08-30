/*
 * @Author: Hanze CHen 
 * @Date: 2021-08-30 22:37:49 
 * @Last Modified by:   Hanze Chen 
 * @Last Modified time: 2021-08-30 22:37:49 
 */
#ifndef P2P_FILE_SYNC_MANAGE_UTILS_COMMAND_UTILS_H
#define P2P_FILE_SYNC_MANAGE_UTILS_COMMAND_UTILS_H

#include <vector>
#include <string>

namespace P2PFileSync {
  using command_type_t = enum {
    front_operation, change_directory, exit_shell, clean_screen
  };

  using command_t = struct command{
    command_t
    std::vector<std::string> argumens;
};
} // P2PFileSync

#endif