
#include <glog/logging.h>

#include <any>
#include <array>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "model/model.hpp"
#include "server_endpoint.h"
#include "server_kit.h"
#include "server_kit_internal.hpp"
#include "utils/curl_utils.hpp"
#include "utils/machine_id.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace fs = std::filesystem;
namespace psk = P2PFileSync::Server_kit;

struct P2P_SYNC_RETURN_t {
  size_t code;
  size_t size;
  void **data;
};

static const constexpr std::array<std::string_view, 4> ERROR_INFO = {
    "return success!",  // ret code 1;
    "failed downloading server information!",
    "failed parsing the downloaded data!",
    "client is already registered!"
};

struct P2P_SYNC_SERVER_HANDLER_t {
  psk::ServerConnection *connection;
};

bool P2PFileSync_SK_success(const P2P_SYNC_RETURN message) {
  return message->code == 0;
};

size_t P2PFileSync_SK_get_data_num(const P2P_SYNC_RETURN message) {
  return message->size;
}

void *P2PFileSync_SK_get_data(const P2P_SYNC_RETURN message,
                              const size_t &data_index) {
  return message->data[data_index];
};

char *P2PFileSync_SK_get_error_message(const P2P_SYNC_RETURN message,
                                       size_t *message_length) {
  auto msg = ERROR_INFO[message->code];
  *message_length = msg.size();
  char *return_val = static_cast<char *>(malloc(*message_length));
  std::memcpy(return_val, msg.cbegin(), *message_length);
  return return_val;
};


void P2PFileSync_SK_global_init(const char *server_url, const char *data_path) {
  psk::ServerConnection::global_init(server_url, data_path);
}

P2P_SYNC_SERVER_SESSION *P2PFileSync_SK_new_session(bool enable_strict_security) {
  auto result = static_cast<P2P_SYNC_SERVER_SESSION *>(
      malloc(sizeof(struct P2P_SYNC_SERVER_HANDLER_t)));

  result->connection = new psk::ServerConnection(enable_strict_security);

  return result;
};

bool P2PFileSync_SK_is_registered() {
  return psk::ServerConnection::is_registered();
}

P2P_SYNC_RETURN P2PFileSync_SK_register_client() {
  // avoid repeat registration
  if(psk::ServerConnection::is_registered()){
    return new P2P_SYNC_RETURN_t{3, 0, nullptr};
  }

  auto client_ret = psk::ServerConnection::regist_client();
  if (client_ret.second == 0) {
    return new P2P_SYNC_RETURN_t{1, 0, nullptr};
  }
  
  // put response to P2P_SYNC_RETURN
  auto ret =static_cast<P2P_SYNC_RETURN>(malloc(sizeof(struct P2P_SYNC_RETURN_t)));

  ret->size = 2;
  ret->data = static_cast<void **>(malloc(sizeof(void *) * 2));

  // stataus
  ret->code = 0;

  // register code
  int register_code = client_ret.second;
  ret->data[0] = static_cast<void *>(malloc(sizeof(int)));
  memcpy(ret->data[0], &register_code, sizeof(int));

  // register link
  auto register_link = client_ret.first;
  ret->data[1] = static_cast<void *>(malloc(register_link.size()));
  memcpy(ret->data[1], register_link.c_str(), register_link.size());

  return ret;
}

#ifdef __cplusplus
}
#endif