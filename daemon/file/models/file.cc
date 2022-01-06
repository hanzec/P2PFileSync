//
// Created by hanzech on 11/27/21.
//

#include "file.h"
#include <glog/logging.h>

namespace P2PFileSync {
FileNode::FileNode(std::shared_ptr<INode> parent_node):_inode(parent_node) {
    _inode->set_file_block(shared_from_this());
}


void FileNode::add_block(const std::string& commit_id,
                         std::shared_ptr<DataBlock>& data_block) {
  std::unique_lock<std::shared_mutex> lock(_mutex);
  _blocks.emplace_back(std::move(data_block));
  _versions.emplace_back(commit_id);
}

void FileNode::add_block(const std::string& commit_id,
                         std::list<std::shared_ptr<DataBlock>>& data_blocks) {
  std::unique_lock<std::shared_mutex> lock(_mutex);
  _blocks.insert(_blocks.end(), data_blocks.begin(), data_blocks.end());
  _versions.emplace_back(commit_id);
}

bool FileNode::replace_block(const std::string& commit_id, size_t index,
                             std::shared_ptr<DataBlock>& block) {
  if (index < 0 || index >= _blocks.size()) {
    LOG(ERROR) << "Data block index out of range";
    return false;
  }

  std::unique_lock<std::shared_mutex> lock(_mutex);
  _blocks[index] = std::move(block);

  _versions.push_back(commit_id);
  return true;
}

bool FileNode::replace_block(const std::string& commit_id,
                             std::vector<std::pair<size_t, std::shared_ptr<DataBlock>>>& block) {
  // detect if the block is already in the file
  for (auto& b : block) {
    if (b.first < 0 || b.first >= _blocks.size()) {
      LOG(ERROR) << "Data block index out of range";
      return false;
    }
  }

  std::unique_lock<std::shared_mutex> lock(_mutex);
  for (auto& b : block) {
    if (b.first > 0 || b.first <= _blocks.size()) {
      // modify the file size
      _file_size += b.second->size();
      _file_size -= _blocks[b.first]->size();

      // replace the block
      _blocks[b.first] = std::move(b.second);
    }
  }
  _versions.push_back(commit_id);
  return true;
}

bool FileNode::add_and_replace_block(const std::string& commit_id, size_t index,
                                     std::vector<std::shared_ptr<DataBlock>>& blocks) {
  if(index < 0 || index >= _blocks.size()) {
    LOG(ERROR) << "Data block start index out of range";
    return false;
  }

  std::unique_lock<std::shared_mutex> lock(_mutex);
  auto final_size = index + blocks.size();
  for (size_t i = index, j = 0; i < final_size; i++, j++) {
    _file_size += blocks[j]->size();

    if(i < _blocks.size()) {
      // modify the file size
      _file_size -= _blocks[i]->size();

      // replace the block
      _blocks[i] = std::move(blocks[j]);
    } else {
      _blocks.emplace_back(std::move(blocks[j]));
    }
  }
  _versions.push_back(commit_id);
  return true;
}
}  // namespace P2PFileSync
