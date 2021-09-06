#include "parsing.h"

#include <algorithm>
#include <cstddef>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "utils/status.h"

namespace P2PFileSync {
Status parsing_command(const std::string& input,
                       COMMAND& parsed_command) {
  std::vector<std::string> result;

  // TODO (chen): will have bugs when multiple spaces, fix later

  // fist words will be command
  size_t current = input.find(' ');
  std::string command = input.substr(0, current);

  size_t next_quotes = 0, next_spaces = 0;
  while (((next_quotes >= next_spaces
               ? (next_quotes = input.find(':', current))
               : next_quotes) != input.npos) ||
         ((next_spaces = input.find(' ', current)) == input.npos)) {

    // if colon is before spaces
    if (next_quotes < next_spaces) {
      auto quotes_end = input.find('\"');

      // if can not found other double quotes
      if (quotes_end == input.npos) {
        return {StatusCode::INVALID_ARGUMENT, 
                generte_nice_error_msg("could not found pair of double quotes",next_quotes, input)};
      }
      
      // fill in result vector
      current = quotes_end;
      result.emplace_back(input.substr(next_quotes + 1, quotes_end - next_quotes));
    }else{
      current = next_spaces;
      result.emplace_back(input.substr(current + 1, next_spaces));
    }
  }

  // set return value
  parsed_command.first = command;
  parsed_command.second = {result.begin(), result.end()};
  return Status::OK();
};

std::string generte_nice_error_msg(const char * error_msg,
                                   const size_t& error_loc,
                                   const std::string& command) {
  size_t final_error_loc;
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

  output << "\033[47;31mError:" << error_msg << "\033[0m"
         << std::endl << final_command << std::endl
         << std::string(final_error_loc - 1,' ') << '^' << std::endl;
  
  return output.str();
}
}  // namespace P2PFileSync