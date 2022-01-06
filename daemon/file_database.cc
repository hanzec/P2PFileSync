//
// Created by hanzech on 11/27/21.
//

#include "file_database.h"

#include <glog/logging.h>

#include <utility>

namespace P2PFileSync {

StorageContext::StorageContext(std::filesystem::path  data_storage_path,
                                         std::shared_ptr<ThreadPool>  thread_pool)
    : _thread_pool(std::move(thread_pool)),
      _data_storage_path(std::move(data_storage_path)) {
  if (!std::filesystem::exists(_data_storage_path)) {
    std::filesystem::create_directories(_data_storage_path);
    LOG(INFO) << "Data storge folder not found, Created: " << _data_storage_path;
  }

}

}  // namespace P2PFileSync
