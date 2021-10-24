
#include <any>
#include <array>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <filesystem>
#include <string_view>
#include <vector>
#include <openssl/pem.h>
#include <glog/logging.h>

#include "server_kit.h"
#include "model/model.hpp"
#include "server_endpoint.h"
#include "utils/machine_id.hpp"
#include "utils/curl_utils.hpp"
#include "server_kit_internal.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace fs = std::filesystem;
namespace psk = P2PFileSync::Server_kit;

struct P2P_SYNC_RETURN_t {
  size_t code;
  size_t size;
  void** data;
};

const std::vector<std::string_view> ERROR_INFO ={
  "return success!", // ret code 1;
};

struct P2P_SYNC_SERVER_HANDLER_t {
  psk::ServerConnection * connection;
};

bool P2PFileSync_SK_success(const P2P_SYNC_RETURN message){
  return message->code == 0;
};

size_t P2PFileSync_SK_get_data_num(const P2P_SYNC_RETURN message){
  return message->size;
}

void * P2PFileSync_SK_get_data(const P2P_SYNC_RETURN message, const size_t& data_index){
  return message->data[data_index];
};

char * P2PFileSync_SK_get_error_message(const P2P_SYNC_RETURN message, size_t * message_length){
  auto msg = ERROR_INFO[message->code];
  *message_length = msg.size();
  char * return_val = static_cast<char*>(malloc(*message_length));
  std::memcpy(return_val, msg.cbegin(), *message_length);
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
    if (!psk::GET_and_save_to_path(nullptr, *psk::server_address_ + psk::SERVER_CERT_ENDPOINT_V1, server_certificate_path, false)) {
      LOG(FATAL) << "failed to download the server certificate from [" << *psk::server_address_ << psk::SERVER_CERT_ENDPOINT_V1  << "]";
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
  psk::RegisterClientRequest reques_model{};

  // TODO: will ip address implement later
  reques_model.setIPAddress("127.0.0.1");
  reques_model.setMachineID(P2PFileSync::Server_kit::get_device_id());

  void * ret_data = psk::POST_and_save_to_ptr(nullptr,*psk::server_address_ + psk::SERVER_REGISTER_ENDPOINT_V1, static_cast<const void*>(reques_model.get_json().c_str()), false);

  // parse response
  psk::RegisterClientResponse response_model(static_cast<char*>(ret_data));

  // put response to P2P_SYNC_RETURN
  auto ret = static_cast<P2P_SYNC_RETURN>(malloc(sizeof(struct P2P_SYNC_RETURN_t)));

  ret->size = 2;
  ret->data = static_cast<void **>(malloc(sizeof(void*)*2));

  // stataus
  ret->code = 0;

  // register code
  int register_code = response_model.get_register_code();
  ret->data[0] = static_cast<void *>(malloc(sizeof(int)));
  memcpy(ret->data[0], &register_code, sizeof(int));

  // register link
  auto register_link = response_model.get_enable_url();
  ret->data[1] = static_cast<void *>(malloc(sizeof(char)*(register_link.size())));
  memcpy(ret->data[1], register_link.c_str(), sizeof(char)*(register_link.size()));

  return ret;
}  


#ifdef __cplusplus
}
#endif