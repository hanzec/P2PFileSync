//
// Created by hanzech on 11/29/21.
//
#include <model/data/device_conf.h>

#include "model/request/register_client_request.h"
#include "server_endpoint.h"
#include "server_kit.h"
#include "utils/curl_utils.h"
#include "utils/ip_address_utils.h"
#include "utils/machine_id.h"

namespace P2PFileSync::ServerKit {
void ServerContext::init(const std::shared_ptr<Config> &config) {
  if (_instance == nullptr) {
    _instance = ServerContext::create(config);
  } else {
    LOG(WARNING) << "ServerContext already initialized";
  }
}

std::shared_ptr<ServerContext> ServerContext::get_server_ctx() { return _instance; }

ServerContext::ServerContext(const this_is_private &,
                             const std::shared_ptr<Config> &config) noexcept
    : _p2p_listen_port(config->p2p_port()),
      _server_address(config->management_server_url()),
      _p2p_listen_interface(config->p2p_listen_interface()),
      _configuration_path(config->config_folder()),
      _client_certificate_path(config->client_certificate_path()),
      _client_configuration_path(config->client_configuration_path()) {}

std::shared_ptr<UserContext> ServerContext::get_usr_ctx() noexcept {
  return UserContext::create(_instance->_server_address);
}

std::shared_ptr<DeviceConfiguration> ServerContext::register_client() {
  if (_instance == nullptr) {
    LOG(ERROR) << "ServerContext not initialized";
    return nullptr;
  }

  // get local ip address
  std::ostringstream oss;
  RegisterClientRequest reques_model;
  auto local_ip = IPAddressUtils::getIPAddresses();

  for (auto &ip : local_ip) {
    if (ip.first == _instance->_p2p_listen_interface) {
      oss << ip.second << ":" << _instance->_p2p_listen_port;
      if (ip != local_ip.back()) oss << ",";
    }
  }
  reques_model.setIPAddress(oss.str());

  // TODO: replace moke data to actual data
  reques_model.setMachineID(P2PFileSync::ServerKit::get_device_id());

  void *raw_json = POST_and_save_to_ptr(
      nullptr, std::string(_instance->_server_address).append(SERVER_REGISTER_ENDPOINT_V1),
      static_cast<const void *>(reques_model.get_json().c_str()), true, false);

  if (raw_json == nullptr) {
    LOG(FATAL) << "Failed to register client";
  }

  // parse response
  RegisterClientResponse resp(static_cast<char *>(raw_json));

  // save to configuration file
  try {
    auto conf = std::make_shared<DeviceConfiguration>(resp);
    LOG(INFO) << "save client configuration to " << _instance->_client_configuration_path;
    conf->save_to_disk(_instance->_client_configuration_path);
    LOG(INFO) << resp.get_enable_url();
    return conf;
  } catch (std::exception &e) {
    LOG(FATAL) << "parse response error: " << e.what();
  }
}

std::shared_ptr<DeviceContext> ServerContext::get_dev_ctx() noexcept {
  if (_instance == nullptr) {
    LOG(ERROR) << "ServerContext not initialized";
    return nullptr;
  }

  if (_instance->_device_ctx_instance == nullptr) {
    std::shared_ptr<DeviceConfiguration> conf{nullptr};
    if (std::filesystem::exists(_instance->_client_configuration_path)) {
      LOG(INFO) << "Device is registered, loading configuration...";
      conf = std::make_shared<DeviceConfiguration>(_instance->_client_configuration_path);
    } else {
      LOG(INFO) << "Device is not registered, generating configuration...";
      conf = register_client();
    }
    _instance->_device_ctx_instance = DeviceContext::create(
        conf->device_id(), conf->jwt_key(), _instance->_server_address,
        _instance->_configuration_path, _instance->_client_certificate_path);
    return _instance->_device_ctx_instance;
  } else {
    VLOG(3) << "DeviceContext already initialized, returning...";
    return _instance->_device_ctx_instance;
  }
}
}  // namespace P2PFileSync::ServerKit
