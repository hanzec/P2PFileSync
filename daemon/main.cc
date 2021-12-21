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
#include "p2p_interface.h"
#include "server_kit/server_kit.h"
#include "utils/data_struct/thread_pool.h"
#include "utils/ip_addr.h"
#include "utils/log.h"
#include "utils/config_reader.h"

DEFINE_string(host, "", "the known host with comma-separated list");
DEFINE_string(config, "", "the location of config file");

using namespace P2PFileSync;
namespace fs = std::filesystem;

int main(int argc, char *argv[], [[maybe_unused]] const char *envp[]) {
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

  std::shared_ptr<Config> config{nullptr};
  if(fs::exists(FLAGS_config)) {
    LOG(INFO) << "load config from " << FLAGS_config;
    config = load_config(FLAGS_config);
    if(!config) {
      LOG(FATAL) << "failed to load config from " << FLAGS_config;
    }
  } else {
    LOG(WARNING) << "config file [" << FLAGS_config << "] not found, will create instead";
    config = std::make_shared<Config>();
    if(save_config(config, FLAGS_config)) {
      LOG(INFO) << "config file [" << FLAGS_config << "] created";
    } else {
      LOG(FATAL) << "failed to create config file [" << FLAGS_config << "]";
    }
  }

  // testing config folder
  if (!fs::exists(config->config_folder())) {
    LOG(WARNING) << "[" << config->config_folder() << "] not exist, will trying to create!";
    if (!std::filesystem::create_directories(config->config_folder()))
      LOG(FATAL) << "failed to create config folder [" << config->config_folder() << "]";
  }


  // start threadl pool
  std::shared_ptr<ThreadPool> thread_pool = std::make_shared<ThreadPool>(config->worker_thread_num());

  // init server context
  ServerKit::ServerContext::init(config);

  // staring server handler
  std::filesystem::path server_sock = config->manage_sock();
  auto manage_ctx = ManagementInterface::init(server_sock, AF_UNIX);

  // starring server pll
  // starting server handler
  if (P2PServerContext::init(config, thread_pool, ServerKit::ServerContext::get_dev_ctx())){
    LOG(INFO) << "p2p server listener is started!";
  } else {
    LOG(FATAL) << "p2p server handler is filed to start";
  }
  // waiting server handler to stop
  P2PServerContext::get()->block_util_server_stop();
  return 0;
}
