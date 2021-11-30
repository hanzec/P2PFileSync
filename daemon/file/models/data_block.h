//
// Created by hanzech on 11/27/21.
//

#ifndef P2P_FILE_SYNC_DATA_BLOCK_H
#define P2P_FILE_SYNC_DATA_BLOCK_H
#include <atomic>
#include <memory>

#include "i_node.h"
namespace P2PFileSync {

class DataBlock : std::enable_shared_from_this<DataBlock> {
 public:
  DataBlock(void* data, uint32_t block_size, int block_index, std::shared_ptr<DataBlock>& prev_block);

  int get_block_index() const;
  std::shared_ptr<DataBlock> get_prev_block() const;
  std::shared_ptr<DataBlock> get_next_block() const;

  uint32_t size();

  std::string_view get_block_hash();

 protected:
  // todo add document
  void set_next_block(std::shared_ptr<DataBlock> next_block);

 private:
  int _block_index;
  uint32_t _block_size;
  std::string_view _block_hash;
  std::shared_ptr<DataBlock> _next_block_ref{nullptr};
  std::shared_ptr<DataBlock> _prev_block_ref{nullptr};
};

}  // namespace P2PFileSync
#endif  // P2P_FILE_SYNC_DATA_BLOCK_H
