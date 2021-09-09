#include "sync.h"

#include <glog/logging.h>

#include <filesystem>

#include "utils/log.h"
#include "utils/status.h"

namespace P2PFileSync {
namespace fs = std::filesystem;

Status start_app(std::vector<IPAddr> known_host,
                 const std::shared_ptr<Config> config) {
  // check data folder
  fs::path data_dir_path(config->get_sync_data_dir());
  if (!fs::exists(data_dir_path)) {
    if (!fs::create_directories(data_dir_path)) {
      LOG(ERROR) << "Failed creating directory in [" << config->get_sync_data_dir() << "]";
      exit(-1);
    }
  } else {
    VLOG(VERBOSE) << "find exist folder in [" << config->get_sync_data_dir() << "]";
  }

  auto premission = fs::status(data_dir_path).permissions();
  if (((premission & fs::perms::owner_read) != fs::perms::none) &&
      ((premission & fs::perms::owner_write) != fs::perms::none)) {
    LOG(ERROR) << "folder [" << config->get_sync_data_dir() << "] need to have both "
               << " read/write permission for user current user";
    exit(-1);
  }
  
  return Status::OK();
}
}  // namespace P2PFileSync