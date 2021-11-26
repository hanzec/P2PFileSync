#include <openssl/pem.h>

#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <variant>

#include "model/model.h"
#include "server_endpoint.h"
#include "server_kit.h"
#include "utils/base64_utils.h"
#include "utils/curl_utils.h"
#include "utils/machine_id.h"

namespace P2PFileSync::Serverkit {

DeviceContext::DeviceContext(std::array<std::byte, 16> client_id, std::string client_token,
                             const std::string &server_address,
                             const std::filesystem::path &conf)
    : _client_id(client_id),
      _login_token(std::move(client_token)),
      _server_address(server_address),
      _server_configuration_path(conf){};

std::shared_ptr<DeviceContext> DeviceContext::get_dev_ctx() noexcept {
  if (_instance == nullptr) {
    return nullptr;
  }
  return _instance->shared_from_this();
}

bool DeviceContext::init_dev_ctx(const std::string &server_address,
                                 const std::filesystem::path &conf) noexcept {
  if (_instance == nullptr) {
    if (is_registered(conf)) {
      DeviceConfiguration conf_file(conf / CLIENT_CONFIGURE_FILE_NAME);
      _instance = std::shared_ptr<DeviceContext>(
          new DeviceContext(conf_file.get_device_id(), conf_file.get_jwt_key(),
          server_address, conf));
      return true;
    } else {
      // c++ 17 unpack pair to variables
      auto param = register_client(server_address, conf);
      _instance = std::shared_ptr<DeviceContext>(
          new DeviceContext(param.second, param.first, server_address, conf));
      return true;
    }
  } else {
    return true;
  }
}

bool DeviceContext::is_enabled() const { return client_info()->success(); }

const std::array<std::byte, 16> &DeviceContext::client_id() const { return _client_id; }

std::unique_ptr<ClientInfoResponse> DeviceContext::client_info() const {
  // client info does not required extra request models

  void *raw_json = GET_and_save_to_ptr(
      nullptr, std::string(_server_address).append(SERVER_REGISTER_ENDPOINT_V1),
      {"Authorization:" + _login_token}, false);

  if (raw_json == nullptr) {
    LOG(ERROR) << "empty response";
    return nullptr;
  }

  // parse response
  try {
    return std::make_unique<ClientInfoResponse>(static_cast<char *>(raw_json));
  } catch (...) {
    return nullptr;
  }
}

std::filesystem::path DeviceContext::client_certificate() const {
  // if the certificate is downloaded
  auto cert_file = _server_configuration_path / CLIENT_CERTIFICATE_FILE_NAME;
  if (!std::filesystem::exists(cert_file)) {
    // if downloaded but not loading
    if (!std::filesystem::exists(cert_file)) {  // download from management server
      void *raw_json = GET_and_save_to_ptr(

          nullptr, std::string(_server_address).append(GET_CLIENT_CERTIFICATE_ENDPOINT_V1),
          {"Authorization:" + _login_token}, false);
      ClientCertResponse resp(static_cast<char *>(raw_json));
      auto cert = Base64::decode(resp.get_client_PSCK12_certificate());

      // first write to file
      std::ofstream file_stream(cert_file, std::ios::out | std::ios::binary);
      file_stream.write(cert.c_str(), cert.size());
      file_stream.close();
    }
  } else {
    // TODO verified before return
  }

  // last return certificate
  return cert_file;
};

std::map<std::string, std::string> DeviceContext::peer_list() const {
  void *raw_json = GET_and_save_to_ptr(
      nullptr, std::string(_server_address).append(GET_CLIENT_PEER_INFO_ENDPOINT_V1),
      {"Authorization:" + _login_token}, false);
  GetPeerListResponse resp(static_cast<char *>(raw_json));
  return resp.get_peer_list();
}

}  // namespace P2PFileSync::Serverkit