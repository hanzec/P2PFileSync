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
ServerContext::ServerContext(const this_is_private &,
                             const std::shared_ptr<Config> &config) noexcept
    : _p2p_listen_port(config->p2p_port()),
      _server_address(config->management_server_url()),
      _p2p_listen_interface(config->p2p_listen_interface()),
      _client_configuration_path(config->client_configuration_path()),
      _client_certificate_path(config->client_certificate_path()),
      _server_certifiacte_path(config->server_certificate_path()) {}

UserContext ServerContext::get_usr_ctx() noexcept {
  return UserContext(get()->_server_address);
}

std::shared_ptr<DeviceConfiguration> ServerContext::register_client() {
  if (get() == nullptr) {
    LOG(ERROR) << "ServerContext not initialized";
    return nullptr;
  }

  // get local ip address
  std::ostringstream oss;
  RegisterClientRequest reques_model;
  auto local_ip = IPAddressUtils::getIPAddresses();

  for (auto &ip : local_ip) {
    if (ip.first == get()->_p2p_listen_interface) {
      oss << ip.second << ":" << get()->_p2p_listen_port;
      if (ip != local_ip.back()) oss << ",";
    }
  }
  reques_model.setIPAddress(oss.str());

  // TODO: replace make data to actual data
  reques_model.setMachineID(P2PFileSync::ServerKit::get_device_id());

  void *raw_json = POST_and_save_to_ptr(
      nullptr, std::string(get()->_server_address).append(SERVER_REGISTER_ENDPOINT_V1),
      static_cast<const void *>(reques_model.get_json().c_str()), true, false);

  if (raw_json == nullptr) {
    LOG(FATAL) << "Failed to register client";
  }

  // parse response
  RegisterClientResponse resp(static_cast<char *>(raw_json));

  // save device configuration
  if(!resp.success()){
    LOG(FATAL) << "Failed to register client: " << resp.message();
  } else {
    auto conf = std::make_shared<DeviceConfiguration>(resp);
    LOG(INFO) << "save client configuration to " << get()->_server_certifiacte_path;
    conf->save_to_disk(get()->_server_certifiacte_path);
    LOG(INFO) << resp.get_enable_url();
    return conf;
  }
}

std::shared_ptr<DeviceContext> ServerContext::get_dev_ctx() noexcept {
  if (get() == nullptr) {
    LOG(ERROR) << "ServerContext not initialized";
    return nullptr;
  }

  if (get()->_device_ctx_instance == nullptr) {
    std::shared_ptr<DeviceConfiguration> conf{nullptr};
    if (std::filesystem::exists(get()->_server_certifiacte_path)) {
      LOG(INFO) << "Device is registered, loading configuration...";
      conf = std::make_shared<DeviceConfiguration>(get()->_server_certifiacte_path);
    } else {
      LOG(INFO) << "Device is not registered, generating configuration...";
      conf = register_client();
    }

    // todo update remote ip address changed if needed
    //    // check if ip address is valid
    //    bool is_valid = false;
    //    std::vector<std::string> changed_ip;
    //    auto conf_ip = conf->device_ip();
    //    for (auto &ip : IPAddressUtils::getIPAddresses()) {
    //      if (ip.first == _instance->_p2p_listen_interface) {
    //        bool match = false;
    //        for(auto &ip_addr : conf_ip) {
    //          if(ip_addr.ip_address() == ip.second) {
    //            match = true;
    //          }
    //        }
    //      }
    //    }

    get()->_device_ctx_instance = std::shared_ptr<DeviceContext>(
        new DeviceContext(conf, get()->_server_address, get()->_client_configuration_path,
                          get()->_client_certificate_path));
    return get()->_device_ctx_instance;
  } else {
    VLOG(3) << "DeviceContext already initialized, returning...";
    return get()->_device_ctx_instance;
  }
}
}  // namespace P2PFileSync::ServerKit
