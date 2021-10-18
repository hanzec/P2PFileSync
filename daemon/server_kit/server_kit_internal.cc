#include "server_kit_internal.hpp"

#include <curl/curl.h>
#include <glog/logging.h>
#include <openssl/pem.h>

#include <algorithm>
#include <filesystem>

#include "server_endpoint.h"
#include "utils/curl_utils.hpp"

namespace fs = std::filesystem;

namespace P2PFileSync::Server_kit {

ServerConnection::ServerConnection(const char* server_address,
                                   const char* certificate_path,
                                   const char* configuration_path)
    : server_address_(server_address),
      certificate_path_(certificate_path),
      configuration_path_(configuration_path) {
  this->init();
};

ServerConnection::ServerConnection(const std::string& server_address,
                                   const std::string& certificate_path,
                                   const std::filesystem::path& configuration_path)
    : server_address_(server_address),
      certificate_path_(certificate_path),
      configuration_path_(configuration_path) {
  this->init();
};

void ServerConnection::init() {
  // initial curl
  if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
    LOG(FATAL) << "failed to init libcurl!";
  }
  if ((curl_handler = curl_share_init()) == nullptr) {
    LOG(FATAL) << "failed to init curl sh!";
  }

  // init trying to find the server certificate
  fs::path server_certificate_path(configuration_path_/SERVER_CERT_NAME);
  if (!fs::exists(server_certificate_path)) {
    LOG(WARNING) << "cannot find the server cert will downloading instead!";
    auto ret = GET_and_save_to_path(curl_handler, server_address_ + SERVER_CERT_ENDPOINT_V1, server_certificate_path, false);
    if (!ret.ok()) {
      LOG(FATAL) << "failed to download the server certificate from [" << server_address_ << SERVER_CERT_ENDPOINT_V1  << "] because " << ret.Message();
    }
  }else{
    FILE * certificate_file = fopen(server_certificate_path.c_str(), "r");
    if((client_sign_certificate_ = PEM_read_PKCS7(certificate_file,nullptr,nullptr,nullptr)) == nullptr){
      LOG(FATAL) << "unable to loading certificate [" << server_certificate_path << "] failed!";
    }else{
      LOG(INFO) << "success loading client sign certificate to memory!";
    }
  }
}

// Status ServerConnection::register_client(){

// };

}  // namespace P2PFileSync::Server_kit