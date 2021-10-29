
#include "server_kit.h"

#include <curl/curl.h>
#include <glog/logging.h>
#include <openssl/pem.h>

#include <algorithm>
#include <filesystem>

#include "server_kit_const.h"
#include "utils/base64_utils.hpp"
#include "utils/curl_utils.hpp"
#include "utils/machine_id.hpp"

namespace fs = std::filesystem;

namespace P2PFileSync::Serverkit {

//------------------------ Global Variable ---------------------------------
// client self identifiers
static const std::string *client_id = nullptr;

// management server information
static const std::string *server_address_ = nullptr;
static const std::filesystem::path *configuration_path_ = nullptr;

// certificates
static const PKCS12 *client_certificate_ = nullptr;
static const PKCS7 *client_sign_certificate_ = nullptr;
//---------------------------------------------------------------------------

bool register_status() {
  // check client cfg
  fs::path client_cfg(*configuration_path_ / CLIENT_CFG_NAME);
  if (!fs::exists(client_cfg)) {
    return false;
  }

  // check client certificate
  if (client_certificate_ == nullptr) {
    fs::path client_cert(*configuration_path_ / CLIENT_CERT_NAME);
    if (!fs::exists(client_cert)) {
      return false;
    }
  }
  return true;
}

// TODO client configuration file and client certificate permission check and
// warring
void global_init(const char *server_url, const char *data_path) {
  // initial curl
  if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
    LOG(FATAL) << "failed to init libcurl!";
  }

  // static global variables
  server_address_ = new std::string(server_url);
  configuration_path_ = new fs::path(data_path);

  // init trying to find the server certificate
  fs::path server_certificate_path(*configuration_path_ / SERVER_CERT_NAME);
  if (!fs::exists(server_certificate_path)) {
    LOG(WARNING) << "cannot find the server cert will downloading instead!";
    if (!GET_and_save_to_path(nullptr,
                              *server_address_ + SERVER_CERT_ENDPOINT_V1,
                              server_certificate_path, false)) {
      LOG(FATAL) << "failed to download the server certificate from ["
                 << *server_address_ << SERVER_CERT_ENDPOINT_V1 << "]";
    }
  } else {
    FILE *certificate_file = fopen(server_certificate_path.c_str(), "r");
    if ((client_sign_certificate_ = PEM_read_PKCS7(
             certificate_file, nullptr, nullptr, nullptr)) == nullptr) {
      LOG(FATAL) << "unable to loading certificate [" << server_certificate_path
                 << "] failed!";
    } else {
      LOG(INFO) << "success loading client sign certificate to memory!";
    }
  }

  // init to find client conf and certificates
  if (register_status()) {
    // load client id
    fs::path client_cfg(*configuration_path_ / CLIENT_CFG_NAME);
    std::ifstream filein(client_cfg);

    std::string tmp;  // TODO need re-write later
    std::getline(filein, tmp);
    client_id = new std::string(tmp);

    // load client certificate
    fs::path client_cert(*configuration_path_ / CLIENT_CERT_NAME);
    FILE *certificate_file = fopen(client_cert.c_str(), "r");
    if ((client_certificate_ = d2i_PKCS12_fp(certificate_file, nullptr)) !=
        nullptr) {
      LOG(FATAL) << "unable to loading certificate [" << client_cert
                 << "] failed!";
    } else {
      LOG(INFO) << "success loading client certificate to memory!";
    }
  }
}

std::shared_ptr<RegisterClientResponse> regist_client() {
  RegisterClientRequest reques_model;

  // TODO: replace moke data to actual data
  srand(time(nullptr));
  reques_model.setIPAddress("127.0.0." + std::to_string(rand() % 255));
  reques_model.setMachineID(P2PFileSync::Serverkit::get_device_id());

  void *raw_json = POST_and_save_to_ptr(
      nullptr, *server_address_ + SERVER_REGISTER_ENDPOINT_V1,
      static_cast<const void *>(reques_model.get_json().c_str()), false);

  if (raw_json == nullptr) {
    LOG(ERROR) << "empty response";
    return nullptr;
  }

  // parse response
  auto resp =
      std::make_shared<RegisterClientResponse>(static_cast<char *>(raw_json));

  // save data to file
  fs::path client_cfg(*configuration_path_ / CLIENT_CFG_NAME);
  std::ofstream client_cfg_file(client_cfg);
  client_cfg_file << resp->get_client_id();
  client_cfg_file.close();

  // check client certificate
  fs::path client_cert(*configuration_path_ / CLIENT_CERT_NAME);
  std::ofstream client_cert_file(client_cert);
  client_cert_file << Base64::decode(resp->get_raw_psck12_certificate());
  client_cert_file.close();

  // load the certificate
  FILE *certificate_file = fopen(client_cert.c_str(), "r");
  if ((client_certificate_ = d2i_PKCS12_fp(certificate_file, nullptr)) !=
      nullptr) {
    LOG(FATAL) << "unable to loading certificate [" << client_cert
               << "] failed!";
  } else {
    LOG(INFO) << "success loading client certificate to memory!";
  }

  return resp;
}

ManagementContext::ManagementContext(bool strict_security)
    : strict_security_(strict_security) {
  if ((curl_handler = curl_share_init()) == nullptr) {
    LOG(FATAL) << "failed to init curl sh!";
  }
};

// Status ServerConnection::register_client(){

// };

}  // namespace P2PFileSync::Serverkit