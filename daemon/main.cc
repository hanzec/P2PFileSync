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
#include "manage_interface.h"
#include "server_kit/server_kit.h"
#include "utils/config_reader.h"
#include "utils/ip_addr.h"
#include "utils/log.h"
#include "utils/status.h"
#include "p2p_interface.h"

DEFINE_string(host, "", "the known host with comma-separated list");
DEFINE_string(config_dir, "", "the location of config file");
DEFINE_string(server, "", "the host name of managed server");

using namespace P2PFileSync;
namespace fs = std::filesystem;

int main(int argc, char *argv[], const char *envp[]) {
  // init logging system
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

//  if (gflags::GetCommandLineFlagInfoOrDie("config_dir").is_default) {
//    LOG(FATAL) << "you must set config_dir";
//  }
//
//  if (gflags::GetCommandLineFlagInfoOrDie("server").is_default) {
//    LOG(FATAL) << "you must set management server";
//  }

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

  auto config = std::make_shared<Config>();


  // testing config folder
  if (!fs::exists(config->config_folder())) {
    LOG(WARNING) << "[" << config->config_folder() << "] not exist, will trying to create!";
    if (!std::filesystem::create_directories(config->config_folder()))
      LOG(FATAL) << "failed to create config folder [" << config->config_folder() << "]";
  }

  // todo fix configuration file later
//  // testing config file
//  auto config_file = data_folder / CONFIG_FILE_NAME;
//  if (!fs::exists(config_file)) {
//    if (!generate_default_config(config_file))
//      LOG(FATAL) << "failed to create default config at [" << config_file.string() << "]";
//  }
//
//  // loading the config file
//  auto config = parse_comfig_file(config_file);
//  if(config == nullptr){
//    LOG(FATAL) << "failed to parse config file at [" << config_file.string() << "]";
//  }

  // check data folder
  fs::path data_dir_path(config->config_folder());
  if (!fs::exists(data_dir_path)) {
    if (!fs::create_directories(data_dir_path)) {
      LOG(FATAL) << "Failed creating directory in [" << config->config_folder() << "]";
    }
  } else {
    VLOG(VERBOSE) << "find exist folder in [" << config->config_folder() << "]";
  }

  // staring server handler
  std::string server_sock = config->get_manage_sock_file_();
  std::thread handler(&manage_interface_thread, std::ref(server_sock), AF_UNIX);

  // initliing server kit
  if(!Serverkit::DeviceContext::init_dev_ctx(config->get_management_server_url(),
                                         config->get_client_config_path())){
    LOG(FATAL) << "failed to init device context!";
  }

  // starting server handler
  if(ProtocolServer::init(config,Serverkit::DeviceContext::get_dev_ctx())){
    LOG(INFO) << "p2p server listener is started!";
  } else {
    LOG(FATAL) << "p2p server handler is filed to start";
  }
  // waiting server handler to stop
  handler.join();
  return 0;
}
