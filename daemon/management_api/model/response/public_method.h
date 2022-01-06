//
// Created by hanzech on 11/30/21.
//

#ifndef P2P_FILE_SYNC_PUBLIC_METHOD_H
#define P2P_FILE_SYNC_PUBLIC_METHOD_H
#include <rapidjson/rapidjson.h>

namespace P2PFileSync::ManagementAPI {
template<typename T>
inline std::unordered_map<int,std::pair<std::string, std::string>> get_user_group_from_node(T& groups){
  static_assert(std::is_base_of<T, rapidjson::Document>::value);
  std::unordered_map<int, std::pair<std::string, std::string>> ret;
  for (size_t idx = 0; idx < groups.Size(); idx++) {
    ret.emplace(groups[idx]["id"].GetInt(),
                std::make_pair(groups[idx]["name"].GetString(), groups[idx]["description"].GetString()));
  }
  return ret;
}
}
#endif  // P2P_FILE_SYNC_PUBLIC_METHOD_H
