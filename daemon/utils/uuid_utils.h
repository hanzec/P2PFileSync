//
// Created by hanzech on 11/24/21.
//

#ifndef P2P_FILE_SYNC_UUID_UTILS_H
#define P2P_FILE_SYNC_UUID_UTILS_H

#include <array>
#include <string>

namespace P2PFileSync::UUID {
// TODO documents
static bool is_valid(const std::string& uuid_string);

// TODO documents
static bool is_valid(const std::array<std::byte, 16>& uuid);

static std::array<std::byte, 16> to_array(const std::string& uuid_string);

static bool compare(const std::array<std::byte, 16>& uuid, const std::string& uuid_string);

};
#endif  // P2P_FILE_SYNC_UUID_UTILS_H