/*
 * @Author: Hanze CHen 
 * @Date: 2021-08-29 22:08:52 
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-08-29 22:14:52
 */
#ifndef P2P_FILE_SYNC_COMMON_H
#define P2P_FILE_SYNC_COMMON_H

#include <cstdint>
#include <string>
#include <utility>

namespace P2PFileSync {

const static std::string CONFIG_FILE_NAME = "config.yaml";

class Config {
 public:
  Config(std::string& mount_point, std::string& sync_data_dir,
         const uint16_t& trans_port_number,std::string& manage_sock_file_)
      : _mount_point(std::move(mount_point)),
        _sync_data_dir(std::move(sync_data_dir)),
        _trans_port_number(trans_port_number),
        manage_sock_file_(std::move(manage_sock_file_)){};

  [[nodiscard]] std::string get_mount_point() const { return _mount_point; }

  [[nodiscard]] std::string get_sync_data_dir() const { return _sync_data_dir; }

  [[nodiscard]] uint16_t get_trans_port_number() const { return _trans_port_number; }

  [[nodiscard]] std::string get_manage_sock_file_() const { return manage_sock_file_; }

 private:
  const uint16_t _trans_port_number;
  const std::string manage_sock_file_;

  const std::string _mount_point;
  const std::string _sync_data_dir;
};
}  // namespace P2PFileSync

#endif