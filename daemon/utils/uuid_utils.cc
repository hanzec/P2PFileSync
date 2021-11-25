//
// Created by hanzech on 11/24/21.
//
#include "uuid_utils.h"

#include <log.h>
#include <uuid/uuid.h>

namespace P2PFileSync::UUID {
// TODO documents
static bool is_valid(const std::string& uuid_string){
  return false;
}

// TODO documents
static bool is_valid(const std::array<std::byte, 16>& uuid){
  return false;
}

static std::array<std::byte, 16> to_array(const std::string& uuid_string){
  std::array<std::byte, 16> ret{static_cast<std::byte>(0)};

  if(uuid_parse(uuid_string.c_str(), (unsigned char*)ret.front()) == 0){
    return ret;
  } else {
    LOG(ERROR) << "parse UUID failed!";
    return ret;
  }

}

static bool compare(const std::array<std::byte, 16>& uuid, const std::string& uuid_string){
  // valid uuid has to be had a length of 16 bytes
  if(uuid_string.size() != uuid.size()){
    return false;
  }

  // check for each bites
  for(size_t idx = 0; idx < uuid.size(); idx ++){
    if(static_cast<uint8_t>(uuid[idx]) != uuid_string.at(idx)){
      return false;
    }
  }

  return true;
}

};
