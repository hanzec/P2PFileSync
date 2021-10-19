#include "command.h"
#include <ctime>   
#include <chrono>

NEW_COMMAND(TIME, "get current time of system", output, args,daemon_status){

  if(args.size() > 0){
    output << "TIME command does not need any arguments!";
  }


  auto end = std::chrono::system_clock::now();
  std::time_t end_time = std::chrono::system_clock::to_time_t(end);

  output << std::ctime(&end_time);
}
