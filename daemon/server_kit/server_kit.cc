#include "server_kit.h"

#include <filesystem>
#include <openssl/pem.h>
#include "server_endpoint.h"
#include "utils/curl_utils.h"
#include "utils/file_handle.h"

namespace P2PFileSync::Serverkit {
// TODO client configuration file and client certificate permission check and
// warring
void global_init(const std::shared_ptr<Config>& client_config) {
  // initial curl
  if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
    LOG(FATAL) << "failed to init libcurl!";
  }

  // static global variables
  _server_address = client_config->get_management_server_url();
  _server_configuration_path = std::filesystem::path(client_config->get_sync_data_dir());
}
} //namespace P2PFileSync::Serverkit