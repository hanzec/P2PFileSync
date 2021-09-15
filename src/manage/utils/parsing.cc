#include "parsing.h"

#include <algorithm>
#include <cstddef>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "utils/status.h"

namespace P2PFileSync {
Status parsing_command(const std::string& input,
                       COMMAND& parsed_command) {

  std::string tmp_input(input);

  // trim the string
  tmp_input.erase(0,tmp_input.find_first_not_of(' '));
  tmp_input.erase(tmp_input.find_last_not_of(' ') + 1);

  if(tmp_input.length() <= 0){
    return {StatusCode::INVALID_ARGUMENT,"command must be not empty"};
  }

  std::vector<std::string> result;

  // fist words will be command
  size_t current = tmp_input.find(' ');
  
  std::string command = tmp_input.substr(0, current);

  while (current != tmp_input.npos) {
    if(tmp_input[current] == '\"'){
      auto quoter_start = current;
      if((current = tmp_input.find('\"',quoter_start+1)) == tmp_input.npos){
         return {StatusCode::INVALID_ARGUMENT, 
            generte_nice_error_msg("could not found pair of double quotes",quoter_start, tmp_input)};
      }else{
        result.emplace_back(tmp_input.substr(quoter_start, current));
      }
    }else{
      auto argument_start = tmp_input.find_first_not_of(' ', current);
      if((current = tmp_input.find_first_of(' ', argument_start + 1)) != tmp_input.npos){
        result.emplace_back(tmp_input.substr(argument_start, current));
      }else{
        result.emplace_back(tmp_input.substr(argument_start, tmp_input.length()));
      }
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