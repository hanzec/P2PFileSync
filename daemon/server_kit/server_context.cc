//
// Created by hanzech on 11/29/21.
//
#include <model/data/device_conf.h>

#include <utility>

#include "server_endpoint.h"
#include "server_kit.h"

namespace P2PFileSync::Serverkit {
void ServerContext::init(const std::string& server_address, const std::filesystem::path& conf) {
  if (_instance == nullptr) {
    _instance = ServerContext::create(server_address, conf);
  }
}

std::shared_ptr<ServerContext> ServerContext::get_server_ctx(){
  return _instance;
}

ServerContext::ServerContext(const this_is_private&, std::string server_address,
                             std::filesystem::path configuration_path) noexcept
    : _server_address(std::move(server_address)),
      _configuration_path(std::move(configuration_path)) {}

std::shared_ptr<UserContext> ServerContext::get_usr_ctx() noexcept {
  return UserContext::create(_instance->_server_address);
}

std::shared_ptr<DeviceContext> ServerContext::get_dev_ctx() noexcept {
  if (_instance->_device_ctx_instance == nullptr) {
    if (std::filesystem::exists(_instance->_configuration_path / CLIENT_CONFIGURE_FILE_NAME)) {
      LOG(INFO) << "Device is registered, loading configuration...";
      DeviceConfiguration conf_file(_instance->_configuration_path /
                                    CLIENT_CONFIGURE_FILE_NAME);
      _instance->_device_ctx_instance =
          DeviceContext::create(conf_file.device_id(), conf_file.jwt_key(),
                                _instance->_server_address, _instance->_configuration_path);
      return _instance->_device_ctx_instance;
    } else {
      LOG(INFO) << "Device is not registered, generating configuration...";
      auto param = register_client(_instance->_server_address, _instance->_configuration_path);
      if (param.first.empty() && param.second.empty()) {
        LOG(ERROR) << "Failed to register device";
        return {nullptr};
      } else {
        _instance->_device_ctx_instance =
            DeviceContext::create(param.second, param.first, _instance->_server_address,
                                  _instance->_configuration_path);
        return _instance->_device_ctx_instance;
      }
    }
  } else {
    return _instance->_device_ctx_instance;
  }
}
}  // namespace P2PFileSync::Serverkit
