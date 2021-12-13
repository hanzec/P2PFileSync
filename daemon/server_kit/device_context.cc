#include <model/response/client/client_certificate_response.h>
#include <model/response/client/client_information_response.h>
#include <model/response/client/get_peer_list_response.h>

#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include "server_endpoint.h"
#include "server_kit.h"
#include "utils/base64_utils.h"
#include "utils/curl_utils.h"

#define MAX_DOWNLOAD_RETIRES 3
#define DOWNLOAD_RETRY_SECOND 10

namespace P2PFileSync::ServerKit {

DeviceContext::DeviceContext(const this_is_private &, std::string device_id,
                             std::string client_token, std::string &server_address,
                             std::filesystem::path &conf)
    : _device_id(std::move(device_id)),
      _login_token(std::move(client_token)),
      _server_address(std::move(server_address)),
      _server_configuration_path(std::move(conf)){};

bool DeviceContext::is_enabled() const { return device_info() != nullptr; }

const std::string &DeviceContext::device_id() const { return _device_id; }

std::unique_ptr<DeviceInfoResponse> DeviceContext::device_info() const {
  // client info does not required extra request models

  if (_login_token.empty()) return nullptr;

  void *raw_json = GET_and_save_to_ptr(
      nullptr, std::string(_server_address).append(GET_CLIENT_INFO_ENDPOINT_V1),
      {"Authorization:" + _login_token}, false);

  if (strlen(static_cast<char *>(raw_json)) == 0) {
    LOG(ERROR) << "empty response";
    return nullptr;
  }

  // parse response
  try {
    return std::make_unique<DeviceInfoResponse>(static_cast<char *>(raw_json));
  } catch (...) {
    return nullptr;
  }
}

std::filesystem::path DeviceContext::client_certificate() const {
  auto cert_file = _server_configuration_path / CLIENT_CERTIFICATE_FILE_NAME;

  for (int retries = 0; retries < MAX_DOWNLOAD_RETIRES; retries++) {
    if (std::filesystem::exists(cert_file)) {
      PKCS12 *cert;
      FILE *file = fopen(cert_file.c_str(), "rb");
      if ((cert = d2i_PKCS12_fp(file, nullptr)) == nullptr) {
        LOG(ERROR) << "unable to loading [" << cert_file << "] will try to re-download in "
                   << DOWNLOAD_RETRY_SECOND << "s";
        sleep(DOWNLOAD_RETRY_SECOND);
        std::filesystem::remove(cert_file);
      } else {
        PKCS12_free(cert);
        LOG(INFO) << "success loading client certificate to memory!";
        return cert_file;
      }
    } else {
      LOG(INFO) << "client certificate [" << cert_file << "] not found, will try to download";
    }

    void *raw_json = GET_and_save_to_ptr(
        nullptr, std::string(_server_address).append(GET_CLIENT_CERTIFICATE_ENDPOINT_V1),
        {"Authorization:" + _login_token}, false);
    ClientCertResponse resp(static_cast<char *>(raw_json));
    auto cert = Base64::decode(resp.PSCK12_certificate());

    // first write to file
    std::ofstream file_stream(cert_file, std::ios::out | std::ios::binary);
    file_stream.write(cert.c_str(), cert.size());
    file_stream.close();
  }

  LOG(FATAL) << "unable to download client certificate after " << MAX_DOWNLOAD_RETIRES
             << " retries";
};

std::map<std::string, std::string> DeviceContext::peer_list() const {
  void *raw_json = GET_and_save_to_ptr(
      nullptr, std::string(_server_address).append(GET_CLIENT_PEER_INFO_ENDPOINT_V1),
      {"Authorization:" + _login_token}, false);
  GetPeerListResponse resp(static_cast<char *>(raw_json));
  return resp.get_peer_list();
}

}  // namespace P2PFileSync::ServerKit