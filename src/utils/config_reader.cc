#include "config_reader.h"

#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

#include "common.h"

namespace P2PFileSync {
[[nodiscard]] bool generate_default_config(const std::string& config_file) {
  static const std::string default_config =
     "app_config:\n                       \
        data_dir: .\\\n                   \
        mount_point: \\p2p_sync\n         \
      network_config:\n                   \
        trans_port_number: 8080\n         \
        manage_port_number: 8081\n        ";

  std::ofstream file(config_file);

  if (file.fail()) {
    return false;
  } else {
    file << default_config << std::endl;
    file.close();
    return true;
  }
}

[[nodiscard]] std::shared_ptr<Config> parse_comfig_file(const std::string& config_file) {
  YAML::Node config = YAML::LoadFile(config_file);
  
  //todo(chen): add config file error checking
  return std::make_shared<Config>(
      config["app_config"]["mount_point"].as<std::string>(),
      config["app_config"]["data_dir"].as<std::string>(),
      config["network_config"]["trans_port_number"].as<uint16_t>(),
      config["network_config"]["manage_port_number"].as<uint16_t>());
}
}  // namespace P2PFileSync