#include "sync.h"

#include <glog/logging.h>

#include <filesystem>

#include "utils/log.h"
#include "utils/status.h"

namespace P2PFileSync {

static Status start_app(std::vector<IPAddr> known_host,
                        const std::shared_ptr<Config> config) {
  return Status::OK();
}
}  // namespace P2PFileSync