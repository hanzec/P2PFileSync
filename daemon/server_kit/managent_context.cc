
#include "server_kit.h"

#include "server_endpoint.h"
namespace fs = std::filesystem;

namespace P2PFileSync::Serverkit {

ManagementContext::ManagementContext(bool strict_security)
    : strict_security_(strict_security) {
  if ((curl_handler = curl_share_init()) == nullptr) {
    LOG(FATAL) << "failed to init curl sh!";
  }
};

// Status ServerConnection::register_client(){

// };

}  // namespace P2PFileSync::Serverkit