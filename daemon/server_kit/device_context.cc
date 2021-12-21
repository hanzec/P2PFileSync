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
#include "utils/ip_address_utils.h"

#define MAX_DOWNLOAD_RETIRES 3
#define DOWNLOAD_RETRY_SECOND 10

namespace P2PFileSync::ServerKit {

DeviceContext::DeviceContext(const std::shared_ptr<DeviceConfiguration> &dev_conf,
                             std::string &server_address,
                             std::filesystem::path &client_configuration_path,
                             std::filesystem::path &client_certificate_path)
    : _device_id(dev_conf->device_id()),
      _login_token(dev_conf->jwt_key()),
      _server_address(server_address),
      _client_certificate_path(std::move(client_certificate_path)),
      _client_configuration_path(std::move(client_configuration_path)){};

bool DeviceContext::is_enabled() const { return device_info() != nullptr; }

const std::string &DeviceContext::device_id() const { return _device_id; }

std::unique_ptr<DeviceInfoResponse> DeviceContext::device_info() const {
  // client info does not required extra request models

  if (_login_token.empty()) return nullptr;

  void *raw_json = GET_and_save_to_ptr(
      nullptr, std::string(_server_address).append(GET_CLIENT_INFO_ENDPOINT_V1),
      {"Authorization:" + _login_token}, false);

  if (strlen(static_cast<char *>(raw_json)) == 0) {
    VLOG(1) << "empty response";
    return nullptr;
  }

  auto response = std::make_unique<DeviceInfoResponse>(static_cast<char *>(raw_json));

  if (response->success()) {
    return response;
  } else {
    VLOG(1) << "failed to get device info: " << response->message();
    return nullptr;
  }
}

std::filesystem::path DeviceContext::client_certificate() const {
  for (int retries = 0; retries < MAX_DOWNLOAD_RETIRES; retries++) {
    if (std::filesystem::exists(_client_certificate_path)) {
      PKCS12 *cert;
      FILE *file = fopen(_client_certificate_path.c_str(), "rb");
      if ((cert = d2i_PKCS12_fp(file, nullptr)) == nullptr) {
        VLOG(1) << "unable to loading [" << _client_certificate_path
                   << "] will try to re-download in " << DOWNLOAD_RETRY_SECOND << "s";
        sleep(DOWNLOAD_RETRY_SECOND);
        std::filesystem::remove(_client_certificate_path);
      } else {
        PKCS12_free(cert);
        LOG(INFO) << "success loading client certificate to memory!";
        return _client_certificate_path;
      }
    } else {
      VLOG(2) << "client certificate [" << _client_certificate_path
                << "] not found, will try to download";
    }

    ClientCertResponse resp(static_cast<char *>(GET_and_save_to_ptr(
        nullptr, std::string(_server_address).append(GET_CLIENT_CERTIFICATE_ENDPOINT_V1),
        {"Authorization:" + _login_token}, false)));

    if (resp.success()) {
      auto cert = Base64::decode(resp.PSCK12_certificate());
      // first write to file
      std::ofstream file_stream(_client_certificate_path, std::ios::out | std::ios::binary);
      file_stream.write(cert.c_str(), cert.size());
      file_stream.close();
    } else {
      VLOG(1) << "failed to download client certificate: " << resp.message();
      continue;
    }
  }
  LOG(FATAL) << "unable to download client certificate after " << MAX_DOWNLOAD_RETIRES
             << " retries";
};

std::unique_ptr<PeerListResponse> DeviceContext::peer_list() const {
  VLOG(3) << "getting peer list";
  auto response = std::make_unique<PeerListResponse>(static_cast<char *>(GET_and_save_to_ptr(
      nullptr, std::string(_server_address).append(GET_CLIENT_PEER_INFO_ENDPOINT_V1),
      {"Authorization:" + _login_token}, false)));

  if (response->success()) {
    return response;
  } else {
    VLOG(1) << "failed to get peer list: " << response->message();
    return nullptr;
  }
}

}  // namespace P2PFileSync::ServerKit