
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>
#include "server_kit.h"
#include "server_kit_internal.hpp"

#ifdef __cplusplus
extern "C" {
#endif

struct P2P_SYNC_RETURN_t {
  char* return_data;
  P2PFileSync::Status return_code;
};

struct P2P_SYNC_SERVER_HANDLER_t {
  P2PFileSync::Server_kit::ServerConnection * connection;
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

P2P_SYNC_SERVER_HANDLER * P2PFileSync_SK_init(const char * server_url, const char * file_path, const char * config_path){
  auto result = static_cast<P2P_SYNC_SERVER_HANDLER*>(malloc(sizeof(struct P2P_SYNC_SERVER_HANDLER_t)));
  try{
    result->connection = new P2PFileSync::Server_kit::ServerConnection(server_url, file_path, config_path);
  }catch(...){
    return nullptr;
  }
  return result;
};

#ifdef __cplusplus
}
#endif