#include "server_kit.h"

#include <openssl/pem.h>

#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <variant>

#include "model/model.h"
#include "server_endpoint.h"
#include "utils/base64_utils.h"
#include "utils/curl_utils.h"
#include "utils/machine_id.h"

namespace P2PFileSync::Serverkit {

DeviceContext::DeviceContext(std::string client_id, std::string client_token)
    : _client_id(std::move(client_id)), _login_token(std::move(client_token)){};

std::shared_ptr<DeviceContext> DeviceContext::get_one() {
  if (_instance == nullptr) {
    if (DeviceContext::is_registered()) {
      DeviceConfiguration conf(_server_configuration_path / CLIENT_CONFIGURE_FILE_NAME);
      return (_instance = std::shared_ptr<DeviceContext>(
                  new DeviceContext(conf.get_device_id(), conf.get_jwt_key())));
    } else {
      // c++ 17 unpack pair to variables
      auto param = DeviceContext::register_client();
      return (_instance = std::shared_ptr<DeviceContext>(
                  new DeviceContext(param.first, param.second)));
    }
  }
  return _instance;
}

bool DeviceContext::is_enabled() const { return client_info()->success(); }

bool DeviceContext::is_registered() {
  // check client cfg
  std::filesystem::path client_cfg(_server_configuration_path / CLIENT_CONFIGURE_FILE_NAME);
  if (!std::filesystem::exists(client_cfg)) {
    return false;
  }

  // check client certificate
  std::filesystem::path client_cert(_server_configuration_path / CLIENT_CERTIFICATE_FILE_NAME);
  if (!std::filesystem::exists(client_cert)) {
    return false;
  }

  return true;
}

const std::string &DeviceContext::client_id() const { return _client_id; }

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

std::pair<std::string, std::string> DeviceContext::register_client() {
  RegisterClientRequest reques_model;

  // TODO: replace moke data to actual data
  srand(time(nullptr));
  reques_model.setIPAddress("127.0.0." + std::to_string(rand() % 255));
  reques_model.setMachineID(P2PFileSync::Serverkit::get_device_id());

  void *raw_json = POST_and_save_to_ptr(
      nullptr, std::string(_server_address).append(SERVER_REGISTER_ENDPOINT_V1),
      static_cast<const void *>(reques_model.get_json().c_str()), false);

  if (raw_json == nullptr) {
    LOG(ERROR) << "empty response";
    return {"", ""};
  }

  // parse response
  RegisterClientResponse resp(static_cast<char *>(raw_json));

  // save to configuration file
  DeviceConfiguration conf(resp);
  conf.save_to_disk(_server_configuration_path / CLIENT_CONFIGURE_FILE_NAME);

  return {resp.get_login_token(), resp.get_client_id()};
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