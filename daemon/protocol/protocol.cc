#include "protocol.h"
#include "utils/thread_pool.h"

namespace P2PFileSync::Protocol {

  ProtocolServer::ProtocolServer(const uint8_t number_worker, const std::filesystem::path& certificate_path):ThreadPool(number_worker){
   FILE *certificate_file = fopen(client_cert.c_str(), "r");
  if ((client_certificate_ = d2i_PKCS12_fp(certificate_file, nullptr)) !=
      nullptr) {
    LOG(FATAL) << "unable to loading certificate [" << client_cert
               << "] failed!";
  } else {
    LOG(INFO) << "success loading client certificate to memory!";
  }
 };
} 