#include "server_kit_internal.hpp"

#include <curl/curl.h>
#include <glog/logging.h>

#include <algorithm>
#include <filesystem>

#include "server_endpoint.h"
#include "utils/curl_utils.hpp"

namespace fs = std::filesystem;

namespace P2PFileSync::Server_kit {

ServerConnection::ServerConnection(bool strict_security) :strict_security_(strict_security) {
  if ((curl_handler = curl_share_init()) == nullptr) {
    LOG(FATAL) << "failed to init curl sh!";
  }
};


// Status ServerConnection::register_client(){

// };

}  // namespace P2PFileSync::Server_kit