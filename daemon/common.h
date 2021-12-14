/*
 * @Author: Hanze CHen
 * @Date: 2021-08-29 22:08:52
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-08-29 22:14:52
 */
#ifndef P2P_FILE_SYNC_COMMON_H
#define P2P_FILE_SYNC_COMMON_H

#include <sys/socket.h>

#include <cstdint>
#include <filesystem>
#include <string>
#include <utility>

namespace P2PFileSync {
class Config {
 public:
  Config()
      : _worker_thread_num(5),
        _packet_cached_size(100),
        _management_server_url("http://10.10.0.118"),
        _p2p_port(3324),
        _p2p_listen_address(),
        _p2p_listen_interface("eth0"),
        _mount_point("/mnt/p2pfs"),
        _config_folder("/tmp/p2p_sync"),
        _manage_sock_url((_config_folder / "sync.sock").string()),
        _client_conf_path(_config_folder / "client.cfg"),
        _client_cert_path(_config_folder / "client.p12"),
        _server_cert_path(_config_folder / "client_sign_root.crt"){};

  Config(uint8_t worker_thread_num, uint32_t packet_cached_size,
         std::string management_server_url, uint16_t p2p_port, std::string p2p_listen_address,
         std::string p2p_listen_interface, const std::string& mount_point,
         const std::string& config_folder, std::string  manage_sock_file,
         const std::string& client_conf_path, const std::string& client_cert_path,
         const std::string& server_cert_path)
      : _worker_thread_num(worker_thread_num),
        _packet_cached_size(packet_cached_size),
        _management_server_url(std::move(management_server_url)),
        _p2p_port(p2p_port),
        _p2p_listen_address(std::move(p2p_listen_address)),
        _p2p_listen_interface(std::move(p2p_listen_interface)),
        _mount_point(mount_point),
        _config_folder(config_folder),
        _manage_sock_url(std::move(manage_sock_file)),
        _client_conf_path(client_conf_path),
        _client_cert_path(client_cert_path),
        _server_cert_path(server_cert_path){};

  [[nodiscard]] uint16_t p2p_port() const { return _p2p_port; }

  [[nodiscard]] std::string p2p_listen_address() const { return _p2p_listen_address; }

  [[nodiscard]] std::string p2p_listen_interface() const { return _p2p_listen_interface; }

  [[nodiscard]] std::filesystem::path mount_point() const { return _mount_point; }

  [[nodiscard]] std::filesystem::path config_folder() const { return _config_folder; }

  [[nodiscard]] std::string manage_sock() const { return _manage_sock_url; }

  [[nodiscard]] std::filesystem::path client_certificate_path() const {
    return _client_cert_path;
  }

  [[nodiscard]] std::filesystem::path server_certificate_path() const {
    return _server_cert_path;
  }

  [[nodiscard]] std::filesystem::path client_configuration_path() const {
    return _client_conf_path;
  }

  [[nodiscard]] uint16_t worker_thread_num() const { return _worker_thread_num; }

  [[nodiscard]] uint32_t packet_cache_size() const { return _packet_cached_size; }

  [[nodiscard]] std::string management_server_url() const { return _management_server_url; }

 private:
  /**
   * General Settings
   */
  const uint8_t _worker_thread_num;
  const uint32_t _packet_cached_size;
  const std::string _management_server_url;  // TODO need to modify her

  /**
   * P2P Settings
   */
  const uint16_t _p2p_port;
  const std::string _p2p_listen_address;
  const std::string _p2p_listen_interface;

  /**
   * Sync Settings
   */
  const std::filesystem::path _mount_point;
  const std::filesystem::path _config_folder;
  const std::string _manage_sock_url;
  const std::filesystem::path _client_conf_path;
  const std::filesystem::path _client_cert_path;
  const std::filesystem::path _server_cert_path;
};
}  // namespace P2PFileSync

#endif