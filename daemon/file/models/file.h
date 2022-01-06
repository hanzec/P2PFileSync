//
// Created by hanzech on 11/27/21.
//

#ifndef P2P_FILE_SYNC_FILE_H
#define P2P_FILE_SYNC_FILE_H
#include <list>
#include <memory>
#include <shared_mutex>
#include <vector>

#include "i_node.h"
#include "data_block.h"

namespace P2PFileSync {

class FileNode : public std::enable_shared_from_this<FileNode> {
 public:
  FileNode(std::shared_ptr<INode> parent_node);

  // todo add documentation
  void add_block(const std::string& commit_id, std::shared_ptr<DataBlock>& data_block);

  // todo add documentation
  void add_block(const std::string& commit_id,
                 std::list<std::shared_ptr<DataBlock>>& data_blocks);

  // todo add documentation
  bool replace_block(const std::string& commit_id, size_t index,
                     std::shared_ptr<DataBlock>& block);

  // todo add documentation
  bool replace_block(const std::string& commit_id,
                     std::vector<std::pair<size_t, std::shared_ptr<DataBlock>>>& blocks);

  // todo add documentation
  bool add_and_replace_block(const std::string& commit_id, size_t index,
                             std::vector<std::shared_ptr<DataBlock>>& blocks);

 private:
  uint64_t _file_size;
  std::shared_mutex _mutex;
  std::shared_ptr<INode> _inode;
  std::list<std::string> _versions;
  std::vector<std::shared_ptr<DataBlock>> _blocks;
};

}  //   namespace P2PFileSync

#endif  // P2P_FILE_SYNC_FILE_H
