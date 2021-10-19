
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string>
#include <filesystem>
#include <openssl/pem.h>
#include <glog/logging.h>

#include "server_kit.h"
#include "server_endpoint.h"
#include "utils/curl_utils.hpp"
#include "server_kit_internal.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace fs = std::filesystem;
namespace psk = P2PFileSync::Server_kit;

struct P2P_SYNC_RETURN_t {
  char* return_data;
  P2PFileSync::Status return_code;
};

struct P2P_SYNC_SERVER_HANDLER_t {
  psk::ServerConnection * connection;
};

bool P2PFileSync_SK_success(const P2P_SYNC_RETURN message){
  return message->return_code.ok();
};

char * P2PFileSync_SK_get_data(const P2P_SYNC_RETURN message){
  return message->return_data;
};

char * P2PFileSync_SK_get_error_message(const P2P_SYNC_RETURN message, size_t * message_length){
  *message_length = message->return_code.Message().size();
  char * return_val = static_cast<char*>(malloc(*message_length));
  std::memcpy(return_val, message->return_code.Message().c_str(), *message_length);
  return return_val;
};

void P2PFileSync_SK_global_init(const char * server_url, const char * data_path){
  // initial curl
  if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
    LOG(FATAL) << "failed to init libcurl!";
  }

  // static global variables
  psk::server_address_ = new std::string(server_url);  
  psk::configuration_path_ = new fs::path(data_path);


  // init trying to find the server certificate
  fs::path server_certificate_path(*psk::configuration_path_/psk::SERVER_CERT_NAME);
  if (!fs::exists(server_certificate_path)) {
    LOG(WARNING) << "cannot find the server cert will downloading instead!";
    auto ret = psk::GET_and_save_to_path(nullptr, *psk::server_address_ + psk::SERVER_CERT_ENDPOINT_V1, server_certificate_path, false);
    if (!ret.ok()) {
      LOG(FATAL) << "failed to download the server certificate from [" << *psk::server_address_ << psk::SERVER_CERT_ENDPOINT_V1  << "] because " << ret.Message();
    }
  }else{
    FILE * certificate_file = fopen(server_certificate_path.c_str(), "r");
    if((psk::client_sign_certificate_ = PEM_read_PKCS7(certificate_file,nullptr,nullptr,nullptr)) == nullptr){
      LOG(FATAL) << "unable to loading certificate [" << server_certificate_path << "] failed!";
    }else{
      LOG(INFO) << "success loading client sign certificate to memory!";
    }
  }
}

P2P_SYNC_SERVER_SESSION * P2PFileSync_SK_new_session(bool enable_strict_security){
  auto result = static_cast<P2P_SYNC_SERVER_SESSION*>(malloc(sizeof(struct P2P_SYNC_SERVER_HANDLER_t)));

  result->connection = new psk::ServerConnection(enable_strict_security);

  return result;
};


P2P_SYNC_RETURN P2PFileSync_SK_register_client(){

}


#ifdef __cplusplus
}
#endif