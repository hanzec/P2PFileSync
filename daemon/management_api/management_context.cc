//
// Created by hanzech on 11/29/21.
//
#include <model/data/device_conf.h>

#include <string_view>

#include "include/management_api.h"
#include "management_api_impl.h"
#include "model/request/register_client_request.h"
#include "server_endpoint.h"
#include "utils/curl_utils.h"
#include "utils/ip_address_utils.h"
#include "utils/machine_id_utils.h"
#include "version.h"

namespace P2PFileSync::ManagementAPI {
/**
 * Keep this as global variable prevent destroy by mistake
 */
static std::shared_ptr<ManagementContext> instance;

/**
 * C++ Interface for ManagementContext
 */
ManagementContext::ManagementContext(const Config &config) noexcept
    : _p2p_listen_port(config.p2p_port()),
      _server_address(config.management_server_url()),
      _p2p_listen_interface(config.p2p_listen_interface()),
      _client_configuration_path(config.client_configuration_path()),
      _client_certificate_path(config.client_certificate_path()),
      _server_certifiacte_path(config.server_certificate_path()) {}

std::unique_ptr<IUserContext> ManagementContext::usr_ctx() noexcept {
  if (instance == nullptr) {
    LOG(ERROR) << "ManagementContext_internal not initialized";
    return nullptr;
  }

  return std::make_unique<UserContextImpl>(instance->_server_address);
}

void ManagementContext::register_client() {
  if (instance == nullptr) {
    LOG(ERROR) << "ManagementContext_internal not initialized";
    return;
  }

  // get local ip address
  std::ostringstream oss;
  RegisterClientRequest reques_model;
  auto local_ip = IPAddressUtils::getIPAddresses();

  for (auto &ip : local_ip) {
    if (ip.first == instance->_p2p_listen_interface) {
      oss << ip.second << ":" << instance->_p2p_listen_port;
      if (ip != local_ip.back()) oss << ",";
    }
  }
  reques_model.setIPAddress(oss.str());

  // TODO: replace make data to actual data
  reques_model.setMachineID(P2PFileSync::ManagementAPI::get_device_id());

  void *raw_json = Curl::POST_and_save_to_ptr(
      nullptr, std::string(instance->_server_address).append(SERVER_REGISTER_ENDPOINT_V1),
      static_cast<const void *>(reques_model.get_json().c_str()), true, false);

  if (raw_json == nullptr) {
    LOG(FATAL) << "Failed to register device";
  }

  // parse response
  RegisterClientResponse resp(static_cast<char *>(raw_json));

  // save device configuration
  if (!resp.success()) {
    LOG(FATAL) << "Failed to register device: " << resp.message();
  } else {
    auto conf = std::make_shared<DeviceConfiguration>(resp);
    LOG(INFO) << "save device configuration to " << instance->_server_certifiacte_path;
    conf->save(instance->_server_certifiacte_path);
    LOG(INFO) << resp.get_enable_url();
    return;
  }
}

std::shared_ptr<IDeviceContext> ManagementContext::dev_ctx() noexcept {
  if (instance == nullptr) {
    LOG(ERROR) << "ManagementContext_internal not initialized";
    return nullptr;
  }

  if (instance->_device_ctx_instance == nullptr) {
    std::shared_ptr<DeviceConfiguration> conf{nullptr};
    if (std::filesystem::exists(instance->_server_certifiacte_path)) {
      LOG(INFO) << "Device is registered, loading configuration...";
      conf = std::make_shared<DeviceConfiguration>(instance->_server_certifiacte_path);
    } else {
      LOG(INFO) << "Device is not registered, generating configuration...";
      register_client();
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

    instance->_device_ctx_instance = std::shared_ptr<DeviceContextImpl>(new DeviceContextImpl(
        conf, instance->_server_address, instance->_client_configuration_path,
        instance->_client_certificate_path));
    return instance->_device_ctx_instance;
  } else {
    VLOG(3) << "_DeviceContext already initialized, returning...";
    return instance->_device_ctx_instance;
  }
}

/**
 * C Interface for ManagementContext
 */
constexpr std::string_view version() { return {VERSION}; }

constexpr std::string_view full_version() { return {FULL_VERSION_WITH_GIT}; }

void init(const Config &config) noexcept {
  instance = std::make_shared<ManagementContext>(config);
};
std::unique_ptr<IUserContext> usr_ctx() noexcept { return instance->usr_ctx(); }

std::shared_ptr<IDeviceContext> dev_ctx() noexcept { return instance->dev_ctx(); }

}  // namespace P2PFileSync::ManagementAPI
