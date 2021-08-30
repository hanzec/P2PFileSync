#include <gflags/gflags.h>

#include <iostream>
#include <regex>
#include <utility>
#include <vector>
#include <filesystem>

#include "common.h"
#include "sync.h"
#include "utils/log.h"
#include "utils/status.h"
#include "utils/ip_addr.h"
#include "utils/config_reader.h"

DEFINE_string(host, "", "the known host with comma-separated list");
DEFINE_string(data_dir, "", "the mounting point of sharing folder");

using namespace P2PFileSync;
namespace fs = std::filesystem;

int main(int argc, char *argv[], const char *envp[]) {
  // init logging system
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, false);

  if (gflags::GetCommandLineFlagInfoOrDie("host").is_default) {
    VLOG(ERROR) << "host parameter has to be set!";
    exit(-1);
  }

  if (gflags::GetCommandLineFlagInfoOrDie("data_dir").is_default) {
    VLOG(ERROR)
        << "you must set a data dir";
  }

  // collect ip addr info
  std::vector<IPAddr> known_host;
  {
    std::stringstream ip_list(FLAGS_host);
    while (ip_list.good()) {
      std::string substr;
      getline(ip_list, substr, ',');

      IPAddr ip(substr);
      if (ip.valid()) {
        known_host.emplace_back(ip);
      } else {
        VLOG(ERROR) << "failed to parsing ip [" << substr << "]";
        exit(-1);
      }
    }
  }

  // check data folder
  fs::path data_dir_path(FLAGS_data_dir);
  if (!fs::exists(data_dir_path)) {
    if (!fs::create_directories(data_dir_path)) {
      LOG(ERROR) << "Failed creating directory in [" << FLAGS_data_dir << "]";
      exit(-1);
    }
  } else {
    VLOG(VERBOSE) << "find exist folder in [" << FLAGS_data_dir << "]";
  }

  auto premission = fs::status(data_dir_path).permissions();
  if (((premission & fs::perms::owner_read) != fs::perms::none) &&
      ((premission & fs::perms::owner_write) != fs::perms::none)) {
    LOG(ERROR) << "folder [" << FLAGS_data_dir << "] need to have both "
               << " read/write permission for user current user";
    exit(-1);
  }

  // loading config file
  auto config_file_path = data_dir_path/CONFIG_FILE_NAME;
  if(!fs::exists(config_file_path)){
    if(!generate_default_config(config_file_path.string())){
      LOG(ERROR) << "failed to create default config file in [" << config_file_path.string()
                 << "] when no config file found!";
      exit(-1);
    }
  }

  auto config = parse_comfig_file(config_file_path.string());

  // run actual sync app, and will block until exit
  auto return_code = start_app(known_host, config);

  // print log if exit with failed
  if (return_code.Code() != StatusCode::OK) {
    VLOG(ERROR) << "app exit with error code [" << return_code.Code()
                << "] and message: " << return_code.Message();
    exit(-1);
  }

  return 0;
}
