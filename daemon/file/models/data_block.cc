//
// Created by hanzech on 11/27/21.
//
#include "data_block.h"

#include <openssl/sha.h>

#include "utils/uuid_util.h"

namespace P2PFileSync {
DataBlock::DataBlock(void* data, uint32_t block_size, int block_index,
                     std::shared_ptr<DataBlock>& prev_block)
    : _block_size(block_size),
      _block_index(block_index),
      _prev_block_ref(std::move(prev_block)) {
  // link nodes to previous block
  prev_block->set_next_block(shared_from_this());

  // generate hash
  unsigned char md[SHA256_DIGEST_LENGTH];  // 32 bytes
  SHA256((unsigned char*)data, block_size, md);
  _block_hash = std::string_view((char*)md, SHA256_DIGEST_LENGTH);
}

// todo need fix ABA problem
void DataBlock::set_next_block(std::shared_ptr<DataBlock> next_block) {
  while (!atomic_compare_exchange_strong(&_next_block_ref, &next_block, next_block))
    ;
}
uint32_t DataBlock::size() { return _block_size; }
int DataBlock::get_block_index() const { return _block_index; }
std::shared_ptr<DataBlock> DataBlock::get_prev_block() const { return _prev_block_ref; }
std::shared_ptr<DataBlock> DataBlock::get_next_block() const { return _next_block_ref; }
std::string_view DataBlock::get_block_hash() { return _block_hash; }

}  // namespace P2PFileSync