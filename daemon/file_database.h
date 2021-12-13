//
// Created by hanzech on 11/27/21.
//

#ifndef P2P_FILE_SYNC_FILE_DATABASE_H
#define P2P_FILE_SYNC_FILE_DATABASE_H
#include <sqlite3.h>
#include <utils/data_struct/thread_pool.h>

#include <filesystem>
#include <string>

namespace P2PFileSync {
class StorageContext {
 protected:
  /**
   * Blocker avoid call public constructor
   */
  struct this_is_private;

 public:
 static bool init(std::filesystem::path  data_storage_path,
                   std::shared_ptr<ThreadPool>  thread_pool);
 protected:
  StorageContext(std::filesystem::path  data_storage_path,
                      std::shared_ptr<ThreadPool>  thread_pool);

  std::future<bool> add_file();
  /**
   * Blocker avoid call public constructor
   */
  struct this_is_private {
    explicit this_is_private(int) {}
  };

  /**
   *
   * @tparam Args
   * @param args
   * @return
   */
  template <typename... Args>
  static ::std::shared_ptr<StorageContext> create(Args&&... args) {
    return ::std::make_shared<StorageContext>(this_is_private{0},
                                                   ::std::forward<Args>(args)...);
  }

 private:
  /**
   * instance of this class
   */
  static std::shared_ptr<StorageContext> _instance;

  std::string _current_head_commit_id;
  std::filesystem::path _data_storage_path;
  std::shared_ptr<ThreadPool> _thread_pool;

};

}  // namespace P2PFileSync

#endif  // P2P_FILE_SYNC_FILE_DATABASE_H
