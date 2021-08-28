#include <gflags/gflags.h>

#include <iostream>
#include <regex>
#include <utility>
#include <vector>

#include "sync.h"
#include "utils/log.h"
#include "utils/status.h"
#include "utils/ip_addr.h"

DEFINE_string(host, "", "the known host with comma-separated list");
DEFINE_string(mount_point, "\\p2p_sync",
              "the mounting point of sharing folder");

using namespace P2PFileSync;

int main(int argc, char *argv[], const char *envp[]) {
  // init logging system
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, false);

  if (gflags::GetCommandLineFlagInfoOrDie("host").is_default) {
    VLOG(ERROR) << "host parameter has to be set!";
    exit(-1);
  }

  if (gflags::GetCommandLineFlagInfoOrDie("mount_point").is_default) {
    VLOG(WARRING)
        << "mount_point not set! will use default location in [\\p2p_sync]";
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

  // run actual sync app, and will block until exit
  auto return_code = start_app(known_host, FLAGS_mount_point);

  // print log if exit with failed
  if (return_code.Code() != StatusCode::OK) {
    VLOG(ERROR) << "app exit with error code [" << return_code.Code()
                << "] and message: " << return_code.Message();
    exit(-1);
  }

  return 0;
}
