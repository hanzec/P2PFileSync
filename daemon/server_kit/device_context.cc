#include <openssl/pem.h>

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <variant>

#include "model/data/data.h"
#include "model/model.h"
#include "model/response/get_peer_list_response.h"
#include "server_endpoint.h"
#include "server_kit.h"
#include "utils/base64_utils.h"
#include "utils/curl_utils.h"
#include "utils/machine_id.h"

namespace P2PFileSync::Serverkit {

DeviceContext::DeviceContext() {
  if (_login_token.empty()) {
    if (DeviceContext::register_status()) {
      DeviceConfiguration conf(*configuration_path_ /
                               CLIENT_CONFIGURE_FILE_NAME);
      _login_token = conf.get_jwt_key();
    } else {
      _login_token = DeviceContext::regist_client();
    }
  }
};

bool DeviceContext::is_enabled() {
  auto ret = get_client_info();

  return get_client_info()->success();
}

std::unique_ptr<ClientInfoResponse> DeviceContext::get_client_info() {
  // client info does not required extra request models

  void *raw_json = GET_and_save_to_ptr(
      nullptr, *server_address_ + SERVER_REGISTER_ENDPOINT_V1,
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

bool DeviceContext::register_status() {
  // check client cfg
  if (_login_token.empty()) {
    std::filesystem::path client_cfg(*configuration_path_ /
                                     CLIENT_CONFIGURE_FILE_NAME);
    if (!std::filesystem::exists(client_cfg)) {
      return false;
    }
  }

  // check client certificate
  if (_client_cert == nullptr) {
    std::filesystem::path client_cert(*configuration_path_ /
                                      CLIENT_CERTIFICATE_FILE_NAME);
    if (!std::filesystem::exists(client_cert)) {
      return false;
    }
  }
  return true;
}

std::string DeviceContext::regist_client() {
  RegisterClientRequest reques_model;

  // TODO: replace moke data to actual data
  srand(time(nullptr));
  reques_model.setIPAddress("127.0.0." + std::to_string(rand() % 255));
  reques_model.setMachineID(P2PFileSync::Serverkit::get_device_id());

  void *raw_json = POST_and_save_to_ptr(
      nullptr, *server_address_ + SERVER_REGISTER_ENDPOINT_V1,
      static_cast<const void *>(reques_model.get_json().c_str()), false);

  if (raw_json == nullptr) {
    LOG(ERROR) << "empty response";
    return "";
  }

  // parse response
  RegisterClientResponse resp(static_cast<char *>(raw_json));

  // save to configuration file
  DeviceConfiguration conf(resp);
  conf.save_to_disk(*configuration_path_ / CLIENT_CONFIGURE_FILE_NAME);

  return resp.get_login_token();
}

const PKCS7 *DeviceContext::get_server_sign_certificate() {
  if (_client_sign_cert == nullptr) {
    auto path = *configuration_path_ / SERVER_CERT_ENDPOINT_V1;
    if (!std::filesystem::exists(path)) {
      LOG(WARNING) << "cannot find the server cert will downloading instead!";
      if (!GET_and_save_to_path(nullptr,
                                *server_address_ + SERVER_CERT_ENDPOINT_V1,
                                path, false)) {
        LOG(FATAL) << "failed to download the server certificate from ["
                   << *server_address_ << SERVER_CERT_ENDPOINT_V1 << "]";
      }
    } else {
      FILE *certificate_file = fopen(path.c_str(), "r");
      if ((_client_sign_cert = PEM_read_PKCS7(certificate_file, nullptr,
                                              nullptr, nullptr)) == nullptr) {
        LOG(FATAL) << "unable to load [" << _client_sign_cert << "]";
      } else {
        LOG(INFO) << "success loading client sign certificate to memory!";
      }
    }
  }

  return _client_sign_cert;
}

const PKCS12 *DeviceContext::get_certificate() {
  // if there is no in-memory copy of certificate need loaded to memory for
  // cache
  if (_client_cert == nullptr) {
    // if downloaded but not loading
    auto cert_file = *configuration_path_ / CLIENT_CERTIFICATE_FILE_NAME;
    if (!std::filesystem::exists(
            cert_file)) {  // download from management server
      void *raw_json = GET_and_save_to_ptr(
          nullptr, *server_address_ + GET_CLIENT_CERTIFICATE_ENDPOINT_V1,
          {"Authorization:" + _login_token}, false);
      ClientCertResponse resp(static_cast<char *>(raw_json));
      auto decoded_certificate =
          Base64::decode(resp.get_client_PSCK12_certificate());

      // first write to file
      std::ofstream file_stream(cert_file, std::ios::out | std::ios::binary);
      file_stream.write(decoded_certificate.c_str(),
                        decoded_certificate.size());
      file_stream.close();
    }

    // loaded to memory
    FILE *file = fopen(cert_file.c_str(), "rb");
    if ((_client_cert = d2i_PKCS12_fp(file, nullptr)) != nullptr) {
      LOG(FATAL) << "unable to loading certificate [" << cert_file << "]!";
    } else {
      LOG(INFO) << "success loading client certificate to memory!";
    }
  }

  // last return certificate
  return _client_cert;
};

std::unordered_map<std::string,std::string> DeviceContext::get_peer_list() {
  void *raw_json = GET_and_save_to_ptr(
      nullptr, *server_address_ + GET_CLIENT_PEER_INFO_ENDPOINT_V1,
      {"Authorization:" + _login_token}, false);
  GetPeerListResponse resp(static_cast<char *>(raw_json));
  return resp.get_peer_list();
}
}  // namespace P2PFileSync::Serverkit