/*
 * @Author: Hanze CHen 
 * @Date: 2021-08-29 22:08:52 
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-08-29 22:14:52
 */
#ifndef P2P_FILE_SYNC_COMMON_H
#define P2P_FILE_SYNC_COMMON_H

#include <string>
#include <cstdint>
#include <utility>
#include <filesystem>
#include <sys/socket.h>

namespace P2PFileSync {

const static std::string CONFIG_FILE_NAME = "config.yaml";

class Config {
 public:
  Config(){};

  Config(std::string mount_point, std::string sync_data_dir,
         const uint16_t trans_port_number,std::string manage_sock_file_)
      : _mount_point(std::move(mount_point)),
        _config_folder(std::move(sync_data_dir)),
        _trans_port_number(trans_port_number),
        _manage_sock_file(std::move(manage_sock_file_)){};

  [[nodiscard]] std::filesystem::path get_mount_point() const { return _mount_point; }

  [[nodiscard]] std::filesystem::path config_folder() const { return _config_folder; }

  [[nodiscard]] std::filesystem::path get_manage_sock_file_() const { return _manage_sock_file; }

  [[nodiscard]] std::filesystem::path get_client_certificate_path() const { return _client_cert_path; }

  [[nodiscard]] std::filesystem::path get_server_certificate_path() const { return _server_cert_path; }

  [[nodiscard]] std::filesystem::path get_client_config_path() const { return _client_conf_path; }

  [[nodiscard]] uint16_t get_trans_port_number() const { return _trans_port_number; }

  [[nodiscard]] uint16_t get_workder_thread_num() const { return _workder_thread_num; }

  [[nodiscard]] uint32_t get_packet_cache_size() const { return _packet_cached_size; }

  [[nodiscard]] std::string get_listen_ip_address() const { return _listen_ip_address; }

  [[nodiscard]] std::string get_management_server_url() const { return _management_server_url; }

 private:
  const uint8_t _workder_thread_num = 5;
  const uint16_t _trans_port_number = 3324;
  const uint32_t _packet_cached_size = 100;
  const std::string _listen_ip_address = "";
  const std::string _trans_interface_name = "";
  const std::string _management_server_url = "http://10.10.0.118"; // TODO need to modify here


  const std::filesystem::path _mount_point;
  const std::filesystem::path _config_folder = "/tmp/sync_data";
  const std::filesystem::path _manage_sock_file = _config_folder /"manage.sock";
  const std::filesystem::path _client_conf_path = _config_folder /"client.cfg";
  const std::filesystem::path _client_cert_path = _config_folder /"client.p12";
  const std::filesystem::path _server_cert_path = _config_folder /"client_sign_root.crt";
};
}  // namespace P2PFileSync

#endif