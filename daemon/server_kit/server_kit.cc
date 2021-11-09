#include "server_kit.h"

#include <filesystem>
#include <openssl/pem.h>
#include "server_endpoint.h"
#include "utils/curl_utils.h"

namespace P2PFileSync::Serverkit {
// TODO client configuration file and client certificate permission check and
// warring
void global_init(const Config& client_config) {
  // initial curl
  if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
    LOG(FATAL) << "failed to init libcurl!";
  }

  // static global variables
  server_address_ = new std::string(client_config.get_management_server_url());
  configuration_path_ = new std::filesystem::path(client_config.get_sync_data_dir());
}
} //namespace P2PFileSync::Serverkit