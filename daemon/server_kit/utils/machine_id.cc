#include "machine_id.h"

#include <string>
#include <ctime>       /* time */
#include <cstdlib>     /* srand, rand */

// TODO need to write algorithm for generating unique device id
std::string P2PFileSync::Serverkit::get_device_id(){
  srand (time(nullptr));
  return "FAKE DEVICE ID" + std::to_string(rand());
}