#include <gflags/gflags.h>
#include <sys/socket.h>

#include <filesystem>
#include <iostream>
#include <regex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "common.h"
#include "daemon_state.h"
#include "server_kit/server_kit.h"
#include "utils/config_reader.h"
#include "utils/ip_addr.h"
#include "utils/log.h"
#include "utils/status.h"
#include "manage/manage.h"

DEFINE_string(host, "", "the known host with comma-separated list");
DEFINE_string(config_dir, "", "the location of config file");
DEFINE_string(server, "", "the host name of managed server");

using namespace P2PFileSync;
namespace fs = std::filesystem;

int main(int argc, char *argv[], const char *envp[]) {
  // init logging system
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (gflags::GetCommandLineFlagInfoOrDie("config_dir").is_default) {
    LOG(FATAL) << "you must set config_dir";
  }

  if (gflags::GetCommandLineFlagInfoOrDie("server").is_default) {
    LOG(FATAL) << "you must set management server";
  }

  // collect ip addr info
  std::vector<IPAddr> known_host;
  if (!gflags::GetCommandLineFlagInfoOrDie("host").is_default) {
    std::stringstream ip_list(FLAGS_host);
    while (ip_list.good()) {
      std::string substr;
      getline(ip_list, substr, ',');

      IPAddr ip(substr);
      if (ip.valid()) {
        known_host.emplace_back(ip);
      } else {
        LOG(FATAL) << "failed to parsing ip [" << substr << "]";
      }
    }
  }

  // testing config folder
  fs::path data_folder(FLAGS_config_dir);
  if (!fs::exists(data_folder)) {
    LOG(WARNING) << "[" << data_folder << "] not exist, will trying to create!";
    if (std::filesystem::create_directories(data_folder))
      LOG(FATAL) << "failed to create config folder [" << data_folder << "]";
  }

  // testing config file
  auto config_file = data_folder / CONFIG_FILE_NAME;
  if (!fs::exists(config_file)) {
    if (!generate_default_config(config_file))
      LOG(FATAL) << "failed to create default config at ["
                 << config_file.string() << "]";
  }

  // loading the config file
  auto config = parse_comfig_file(config_file);

  // check data folder
  fs::path data_dir_path(config->get_sync_data_dir());
  if (!fs::exists(data_dir_path)) {
    if (!fs::create_directories(data_dir_path)) {
      LOG(FATAL) << "Failed creating directory in ["
                 << config->get_sync_data_dir() << "]";
    }
  } else {
    VLOG(VERBOSE) << "find exist folder in [" << config->get_sync_data_dir()
                  << "]";
  }

  // global init server management kit
  P2PFileSync_SK_global_init(FLAGS_host.c_str(), FLAGS_config_dir.c_str());

  // trying to look for register devices
  fs::path client_id = data_folder/"client.json";
  fs::path client_certificate = data_folder/"client_certificate.pem";
  if(!(fs::exists(client_id) && fs::exists(client_certificate))){
    DaemonStatus::set_register_status(false);
  }

  // staring server handler
  auto main_handler = std::thread(&P2PFileSync_start_manage_thread, config->get_manage_sock_file_().c_str(), AF_FILE);
  
  // waiting server handler to stop
  main_handler.join();
  return 0;
}
