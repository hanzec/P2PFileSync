//
// Created by hanzech on 11/25/21.
//
#include <model/data/device_conf.h>
#include <model/request/register_client_request.h>
#include <model/response/client/register_client_response.h>
#include <utils/machine_id.h>

#include <filesystem>

#include "server_endpoint.h"
#include "utils/curl_utils.h"
#include "utils/ip_address_utils.h"

namespace P2PFileSync::ServerKit {
bool is_registered(const std::filesystem::path &configuration_path) {
  // check client cfg
  std::filesystem::path client_cfg(configuration_path / CLIENT_CONFIGURE_FILE_NAME);
  if (!std::filesystem::exists(client_cfg)) {
    return false;
  }

  // check client certificate
  std::filesystem::path client_cert(configuration_path / CLIENT_CERTIFICATE_FILE_NAME);
  if (!std::filesystem::exists(client_cert)) {
    return false;
  }

  return true;
}

std::pair<std::string, std::string> register_client(
    const std::string &server_address, const std::filesystem::path &configuration_path) {
  RegisterClientRequest reques_model;

  // get local ip address
  auto local_ip = IPAddressUtils::getIPAddresses();
  std::ostringstream oss;
  std::copy(local_ip.begin(), local_ip.end()-1, std::ostream_iterator<std::string>(oss, ","));
  oss << local_ip.back();
  reques_model.setIPAddress(oss.str());

  // TODO: replace moke data to actual data
  reques_model.setMachineID(P2PFileSync::ServerKit::get_device_id());

  void *raw_json = POST_and_save_to_ptr(
      nullptr, std::string(server_address).append(SERVER_REGISTER_ENDPOINT_V1),
      static_cast<const void *>(reques_model.get_json().c_str()), true,false);

  if (raw_json == nullptr) {
    LOG(ERROR) << "empty response";
    return {"", {}};
  }

  // parse response
  RegisterClientResponse resp(static_cast<char *>(raw_json));

  // save to configuration file
  try{
    std::filesystem::path client_cfg = configuration_path / CLIENT_CONFIGURE_FILE_NAME;
    DeviceConfiguration conf(resp);
    LOG(INFO) << "save client configuration to " << client_cfg;

    conf.save_to_disk(client_cfg);


    LOG(INFO) << resp.get_enable_url();
    return {resp.jwt_key(), resp.device_id()};
  } catch (std::exception &e) {
    LOG(ERROR) << "parse response error: " << e.what();
    return {"", {}};
  }
}

}  // namespace P2PFileSync::ServerKit