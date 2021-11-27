/*
 * @Author: Hanze CHen
 * @Date: 2021-08-30 22:37:49
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-10-23 16:57:47
 */
#ifndef P2P_FILE_SYNC_UTILS_PARSING_H
#define P2P_FILE_SYNC_UTILS_PARSING_H

#include <string>
#include <utility>
#include <vector>

#include "status.h"

namespace P2PFileSync {
using COMMAND = std::pair<std::string, std::vector<std::string>>;

/**
 * @brief Parsring from raw string to command and its arguments vector
 * 
 * @param input the raw string wants to parsed
 * @param parsed_command generated parsed string
 * @return Status the return code while finished parsing
 */
[[nodiscard]] Status parsing_command(const std::string& input, COMMAND& parsed_command);

/**
 * @brief
 *
 * @param error_msg
 * @param error_loc
 * @param command
 * @return std::string
 */
[[nodiscard]] std::string generte_nice_error_msg(const char* error_msg,
                                                 const size_t& error_loc,
                                                 const std::string& command);

}  // namespace P2PFileSync

#endif // P2P_FILE_SYNC_UTILS_PARSING_H