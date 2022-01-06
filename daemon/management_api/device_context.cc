
#include <filesystem>
#include <memory>
#include <string>
#include <utility>

#include "management_api_impl.h"
#include "server_endpoint.h"
#include "utils/base64_utils.h"
#include "utils/curl_utils.h"
#include "model/response/device/device_information_response_impl.h"
#include "model/response/device/device_certificate_response.h"
#include "model/response/device/get_peer_list_response_impl.h"

#define MAX_DOWNLOAD_RETIRES 3
#define DOWNLOAD_RETRY_SECOND 10

namespace P2PFileSync::ManagementAPI {

DeviceContextImpl::DeviceContextImpl(const std::shared_ptr<DeviceConfiguration> &dev_conf,
                             std::string &server_address,
                             std::filesystem::path &client_configuration_path,
                             std::filesystem::path &client_certificate_path)
    : _device_id(dev_conf->device_id()),
      _login_token(dev_conf->jwt_key()),
      _server_address(server_address),
      _client_certificate_path(std::move(client_certificate_path)),
      _client_configuration_path(std::move(client_configuration_path)){};

bool DeviceContextImpl::is_enabled() const { return device_info() != nullptr; }

const std::string &DeviceContextImpl::device_id() const { return _device_id; }

std::unique_ptr<IDeviceInfoResponse> DeviceContextImpl::device_info() const {
  // device info does not required extra request models

  if (_login_token.empty()) return nullptr;

  void *raw_json = Curl::GET_and_save_to_ptr(
      nullptr, std::string(_server_address).append(GET_CLIENT_INFO_ENDPOINT_V1),
      {"Authorization:" + _login_token}, false);

  if (strlen(static_cast<char *>(raw_json)) == 0) {
    VLOG(1) << "empty response";
    return nullptr;
  }

  auto response = std::make_unique<DeviceInfoResponseImpl>(static_cast<char *>(raw_json));

  if (response->success()) {
    return response;
  } else {
    VLOG(1) << "failed to get device info: " << response->message();
    return nullptr;
  }
}

std::filesystem::path DeviceContextImpl::client_certificate() const {
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
        LOG(INFO) << "success loading device certificate to memory!";
        return _client_certificate_path;
      }
    } else {
      VLOG(2) << "device certificate [" << _client_certificate_path
              << "] not found, will try to download";
    }

    ClientCertResponse resp(static_cast<char *>(Curl::GET_and_save_to_ptr(
        nullptr, std::string(_server_address).append(GET_CLIENT_CERTIFICATE_ENDPOINT_V1),
        {"Authorization:" + _login_token}, false)));

    if (resp.success()) {
      // first write to file
      auto cert = Base64::decode(resp.PSCK12_certificate());
      std::ofstream file_stream(_client_certificate_path, std::ios::out | std::ios::binary);
      file_stream.write(cert.c_str(), cert.size());
      file_stream.close();
    } else {
      VLOG(1) << "failed to download device certificate: " << resp.message();
      continue;
    }
  }
  LOG(FATAL) << "unable to download device certificate after " << MAX_DOWNLOAD_RETIRES
             << " retries";
};

std::unique_ptr<IPeerListResponse> DeviceContextImpl::peer_list() const {
  VLOG(3) << "getting peer list";
  auto response =
      std::make_unique<PeerListResponseImpl>(static_cast<char *>(Curl::GET_and_save_to_ptr(
          nullptr, std::string(_server_address).append(GET_CLIENT_PEER_INFO_ENDPOINT_V1),
          {"Authorization:" + _login_token}, false)));

  if (response->success()) {
    return response;
  } else {
    VLOG(1) << "failed to get peer list: " << response->message();
    return nullptr;
  }
}

}  // namespace P2PFileSync::ManagementAPI