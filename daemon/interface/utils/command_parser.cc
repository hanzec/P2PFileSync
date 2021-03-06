/*
 * @Author: Hanze CHen
 * @Date: 2021-10-23 16:57:22
 * @Last Modified by:   Hanze Chen
 * @Last Modified time: 2021-10-23 16:57:22
 */
#include "command_parser.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace P2PFileSync {
Status parse_command(const std::string& input_command, ParsedCommand& parsed_command) {
  std::string input(input_command);

  // fix the string format
  input.erase(0, input.find_first_not_of(' '));
  input.erase(input.find_last_not_of(' ') + 1);
  input.erase(input.find_last_not_of('\n') + 1);

  if (input.length() <= 0) {
    return {StatusCode::INVALID_ARGUMENT, "command must be not empty"};
  }
  std::string command;
  std::vector<std::string> result;

  // todo bad logic here
  size_t current = input.find(' ');
  if (current == std::string::npos) {  // if only command
    // get the command
    command = input;
  } else {
    // get the command
    command = input.substr(0, current);
    input.erase(0, current + 1);

    // get the arguments
    while (std::string::npos != (current = input.find(' '))) {
      if (input[0] == '\"') {
        auto quoter_start = current;
        if ((current = input.find('\"', quoter_start + 1)) == std::string::npos) {
          return {StatusCode::INVALID_ARGUMENT,
                  generate_nice_error_msg("could not found pair of double quotes",
                                          quoter_start, input)};
        } else {
          result.emplace_back(input.substr(quoter_start, current));
        }
      } else {
        result.emplace_back(input.substr(0, current));
        input.erase(0, current + 1);
      }
    }
    result.emplace_back(input);
  }

  // set return value
  parsed_command.first = command;
  parsed_command.second = {result.begin(), result.end()};
  return Status::OK();
};

std::string generate_nice_error_msg(const char* error_msg, const size_t& error_loc,
                                   const std::string& command) {
  size_t final_error_loc = 0;
  std::string final_command;
  std::ostringstream output;

  /**
   * since by default the terminal will be 80*24, so the raw command will
   * keep at most 80 charaters per line for better reading
   */
  if (command.length() >= 80) {
    // if error location is in first 77 characters then print first 77
    // characters end with three dots
    if (error_loc < 77) {
      final_error_loc = error_loc;
      final_command = command.substr(0, 77) + "...";
    } else if ((command.length() - error_loc) < 77) {
      final_error_loc = error_loc - command.length() - 77;
      final_command = command.substr(command.length() - 77, command.length());
    } else {
      // TODO (chen): will do when necessary
    }
  } else {
    final_command = command;
    final_error_loc = error_loc;
  }

  output << "\033[47;31mError:" << error_msg << "\033[0m" << std::endl
         << final_command << std::endl
         << std::string(final_error_loc - 1, ' ') << '^' << std::endl;

  return output.str();
}
}  // namespace P2PFileSync