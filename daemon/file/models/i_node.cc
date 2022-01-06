//
// Created by hanzech on 11/27/21.
//

#include "i_node.h"

#include <utility>

namespace P2PFileSync {

INode::INode(std::shared_ptr<INode> parent, std::string name, int64_t size, int64_t mtime,
             int64_t ctime, int64_t atime, int64_t mode, int64_t uid, int64_t gid,
             int64_t nlink, int64_t inode, int64_t dev)
    : _parent(std::move(parent)),
      _name(std::move(name)),
      size_(size),
      mtime_(mtime),
      ctime_(ctime),
      atime_(atime){

  // if not root add self to parent's children
  if (!is_root()) _parent->add_child(name,shared_from_this());


}

bool INode::is_root() const { return _parent == nullptr; }

bool INode::add_child(const std::string& path, std::shared_ptr<INode> new_child) {
  std::unique_lock<std::shared_mutex> sharedLock(_lock_mutex);

  auto ret = _children.find(path);

  // check if the path already exists
  if ((ret != _children.end()) && (ret->second != new_child)) {
    return false;
  }

  _children.emplace(std::make_pair(path, std::move(new_child)));

  return true;
}
// namespace P2PFileSync

}  // namespace P2PFileSync