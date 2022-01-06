//
// Created by hanzech on 11/27/21.
//

#ifndef P2P_FILE_SYNC_I_NODE_H
#define P2P_FILE_SYNC_I_NODE_H

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

#include "file.h"

namespace P2PFileSync {
class INode : public std::enable_shared_from_this<INode> {
 public:
  enum INodeType {
    kINodeTypeFile = 0,
    kINodeTypeDirectory = 1,
    kINodeTypeFileHardLink = 2,
  };

  INode(std::shared_ptr<INode> parent, std::string name, int64_t size, int64_t mtime,
        int64_t ctime, int64_t atime, int64_t mode, int64_t uid, int64_t gid, int64_t nlink,
        int64_t inode, int64_t dev);

  bool is_root() const;

  void update_atime(int64_t atime);
  void update_mtime(int64_t mtime);
  void update_ctime(int64_t ctime);

  bool set_file_block(std::shared_ptr<FileNode> new_file);

 protected:

  bool add_child(const std::string& path, std::shared_ptr<INode> new_child);
 private:
  uint64_t size_;
  int64_t mtime_;
  int64_t ctime_;
  int64_t atime_;
  std::string_view _name;
  std::shared_mutex _lock_mutex;
  std::shared_ptr<INode> _parent;
  std::shared_ptr<FileNode> _file_block{nullptr};
  std::unordered_map<std::string_view, std::shared_ptr<INode>> _children;
};
}  // namespace P2PFileSync

#endif  // P2P_FILE_SYNC_I_NODE_H
