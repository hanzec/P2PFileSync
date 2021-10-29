#ifndef P2P_FILE_SYNC_PROTOCOL_PROTOCOL_H
#define P2P_FILE_SYNC_PROTOCOL_PROTOCOL_H
#include <future>
#include <filesystem>
#include <openssl/pkcs12.h>

#include "utils/thread_pool.h"

namespace P2PFileSync::Protocol {
class ProtocolServer: private ThreadPool {
 public:
  ProtocolServer(const uint8_t number_worker, const std::filesystem::path& certificate_path);
 protected:

  auto execute();
 private:
  PKCS12 * _certificate;

};
}  // namespace P2PFileSync::Protocol
#endif