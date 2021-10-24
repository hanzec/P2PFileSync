#include "machine_id.hpp"
#include <string>
#include <time.h>       /* time */
#include <stdlib.h>     /* srand, rand */

// TODO need to write algorithm for generating unique device id
std::string P2PFileSync::Server_kit::get_device_id(){
  srand (time(nullptr));
  return "FAKE DEVICE ID" + std::to_string(rand());
}