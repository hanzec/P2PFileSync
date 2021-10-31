#include "protocol.h"

#include <glog/logging.h>

#include <cstddef>
#include <filesystem>
#include <regex>

#include "utils/thread_pool.h"

namespace P2PFileSync::Protocol {
// get instance of ProtocolServer object
std::shared_ptr<ProtocolServer> ProtocolServer::get_instance() {
  return _instance;
};

// initlized protol instance
bool ProtocolServer::init(const uint8_t number_worker,
                          const std::string& listen_address,
                          const std::filesystem::path& certificate_path) {
  // prevent init twice
  if (_instance != nullptr) return true;

  // first check with certificate is exist or not
  if (!std::filesystem::exists(certificate_path)) return false;

  // check the listen address is in correct format
  std::regex ip_pattern(
      R"((?:25[0-5]|2[0-4]\d|[0-1]?\d{1,2})(?:\.(?:25[0-5]|2[0-4]\d|[0-1]?\d{1,2})){3}:(\d)+)");
  if (!std::regex_match(listen_address, ip_pattern)) return false;

  // creating listen connection


  // creating instance
  _instance = std::make_shared<ProtocolServer>(number_worker, listen_address,
                                               certificate_path);

  return true;
};

ProtocolServer::ProtocolServer(const uint8_t number_worker, const int fd,
                               const std::filesystem::path& certificate_path)
    : ThreadPool(number_worker) {
  // loading certificate to memory
  FILE* certificate_file = fopen(certificate_path.c_str(), "r");
  if ((_certificate = d2i_PKCS12_fp(certificate_file, nullptr)) != nullptr) {
    LOG(FATAL) << "unable to loading certificate [" << certificate_path
               << "] failed!";
  } else {
    LOG(INFO) << "success loading client certificate to memory!";
  }

  //
};
}  // namespace P2PFileSync::Protocol