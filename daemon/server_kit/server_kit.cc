//
// Created by hanzech on 11/25/21.
//
#include <model/data/device_conf.h>
#include <model/request/register_client_request.h>
#include <model/response/register_client_response.h>
#include <utils/machine_id.h>

#include <filesystem>

#include "server_endpoint.h"
#include "utils/curl_utils.h"

namespace P2PFileSync::Serverkit {
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

  // TODO: replace moke data to actual data
  srand(time(nullptr));
  reques_model.setIPAddress("127.0.0." + std::to_string(rand() % 255));
  reques_model.setMachineID(P2PFileSync::Serverkit::get_device_id());

  void *raw_json = POST_and_save_to_ptr(
      nullptr, std::string(server_address).append(SERVER_REGISTER_ENDPOINT_V1),
      static_cast<const void *>(reques_model.get_json().c_str()), false);

  if (raw_json == nullptr) {
    LOG(ERROR) << "empty response";
    return {"", {}};
  }

  // parse response
  RegisterClientResponse resp(static_cast<char *>(raw_json));

  // save to configuration file
  try{
    DeviceConfiguration conf(resp);
    LOG(INFO) << "save client configuration to " << configuration_path;

    conf.save_to_disk(configuration_path);


    LOG(INFO) << resp.get_enable_url();
    return {resp.jwt_key(), resp.device_id()};
  } catch (std::exception &e) {
    LOG(ERROR) << "parse response error: " << e.what();
    return {"", {}};
  }
}

}  // namespace P2PFileSync::Serverkit