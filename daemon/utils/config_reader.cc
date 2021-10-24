/*
 * @Author: Hanze CHen 
 * @Date: 2021-10-23 16:57:02 
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-10-23 17:10:24
 */
#include "config_reader.h"

#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include "common.h"

namespace P2PFileSync {
[[nodiscard]] bool generate_default_config(const std::filesystem::path& config_file) {
  static const std::string default_config =
     "app_config:\n\tdata_dir: \"/tmp/p2p_sync/data\"\n\tmount_point: \"/p2p_sync\"\network_config:\n\ttrans_port_number: 8080\n\tmanage_sock_file: \"/tmp/p2p_sync/manage.sock\"\n";
  std::ofstream file(config_file, std::fstream::out);

  if (file.fail()) {
    return false;
  } else {
    file << default_config << std::endl;
    file.close();
    return true;
  }
}

[[nodiscard]] std::shared_ptr<Config> parse_comfig_file(const std::filesystem::path& config_file) {
  YAML::Node config = YAML::LoadFile(config_file.string());
  
  //TODO: add config file error checking
  return std::make_shared<Config>(
      config["app_config"]["mount_point"].as<std::string>(),
      config["app_config"]["data_dir"].as<std::string>(),
      config["network_config"]["trans_port_number"].as<uint16_t>(),
      config["network_config"]["manage_sock_file"].as<std::string>());
}
}  // namespace P2PFileSync