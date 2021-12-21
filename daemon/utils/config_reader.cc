/*
 * @Author: Hanze CHen
 * @Date: 2021-10-23 16:57:02
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-10-23 22:06:10
 */
#include "config_reader.h"

#include <glog/logging.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

#include <cstdint>
#include <fstream>
#include <string>

#include "../common.h"

namespace P2PFileSync {
[[nodiscard]] bool save_config(const std::shared_ptr<Config>& config,
                               const std::filesystem::path& config_file) {
  YAML::Node yaml_config;
  std::ofstream file(config_file, std::fstream::out);

  if (!file.is_open()) {
    LOG(ERROR) << "Failed to open config file: " << config_file;
    return false;
  }

  /**
   * General Settings
   */
  yaml_config["worker_thread"] = config->worker_thread_num();
  yaml_config["packet_cache_size"] = config->packet_cache_size();
  yaml_config["management_server_url"] = config->management_server_url();

  /**
   * P2P Settings
   */
  yaml_config["p2p"]["p2p_port"] = config->p2p_port();
  yaml_config["p2p"]["p2p_listen_address"] = config->p2p_listen_address();
  yaml_config["p2p"]["p2p_listen_interface"] = config->p2p_listen_interface();

  /**
   * Sync Settings
   */
  yaml_config["sync"]["manage_sock"] = config->manage_sock();
  yaml_config["sync"]["mount_point"] = config->mount_point().string();
  yaml_config["sync"]["config_folder"] = config->config_folder().string();
  yaml_config["sync"]["server_certificate_path"] = config->server_certificate_path().string();
  yaml_config["sync"]["client_certificate_path"] = config->client_certificate_path().string();
  yaml_config["sync"]["client_configuration_path"] =
      config->client_configuration_path().string();

  if (file.fail()) {
    return false;
  } else {
    file << yaml_config << std::endl;
    file.close();
    return true;
  }
}

[[nodiscard]] std::shared_ptr<Config> load_config(const std::filesystem::path& config_file) {
  YAML::Node config;
  try {
    config = YAML::LoadFile(config_file.string());
  } catch (YAML::Exception& e) {
    LOG(ERROR) << "Failed to parse config file: " << e.what();
    return {nullptr};
  }

  // TODO: add config file error checking
  return std::make_shared<Config>(
      config["worker_thread"].as<int>(),
      config["packet_cache_size"].as<uint32_t>(),
      config["management_server_url"].as<std::string>(),
      config["p2p"]["p2p_port"].as<uint16_t>(),
      config["p2p"]["p2p_listen_address"].as<std::string>(),
      config["p2p"]["p2p_listen_interface"].as<std::string>(),
      config["sync"]["mount_point"].as<std::string>(),
      config["sync"]["config_folder"].as<std::string>(),
      config["sync"]["manage_sock"].as<std::string>(),
      config["sync"]["client_configuration_path"].as<std::string>(),
      config["sync"]["client_certificate_path"].as<std::string>(),
      config["sync"]["server_certificate_path"].as<std::string>());
}
}  // namespace P2PFileSync