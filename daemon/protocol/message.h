/*
 * @Author: Hanze CHen
 * @Date: 2021-08-28 19:23:53
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-08-29 15:57:02
 */
#ifndef P2P_FILE_SYNC_MESSAGE_MESSAGE_H
#define P2P_FILE_SYNC_MESSAGE_MESSAGE_H

#include <cstdint>
#include <ostream>
#include <ip_addr.h>
#include "payload.pb.h"
#include <google/protobuf/stubs/port.h>

namespace P2PFileSync {
class Message {
 public:
  Message();

  class Payload {
   public:
    Payload();

   private:
    ProtoRequestPayload _payload;
  };

  /**
   * @brief Header structure for message header
   *
   */
  class Header {
   public:
    Header(const IPAddr& src_ip_addr, const IPAddr& des_ip_addr)
        : _source_ip(src_ip_addr), _destination_ip(des_ip_addr){};

    /**
     * @brief Set the source ip with IPAddr class
     *
     * @note from IPAddr class will not check ip address is valid which means
     * using this function will always return true
     * @param ip_addr the incoming ip address will be set
     * @return true IP addr is successful set and valid
     * @return false setting up IP addr is failed and invalid
     */
    bool set_src_ip(const IPAddr& ip_addr);

    /**
     * @brief Set the source ip with raw string
     *
     * @param ip_addr the incoming ip address will be set
     * @return true IP addr is successful set and valid
     * @return false setting up IP addr is failed and invalid
     */
    bool set_src_ip(const std::string& ip_addr);

    /**
     * @brief Set the des ip with IPAddr class
     *
     * @note from IPAddr class will not check ip address is valid which means
     * using this function will always return true
     * @param ip_addr the incoming ip address will be set
     * @return true IP addr is successful set and valid
     * @return false setting up IP addr is failed and invalid
     */
    bool set_des_ip(const IPAddr& ip_addr);

    /**
     * @brief Set the des ip with raw string
     *
     * @param ip_addr the incoming ip address will be set
     * @return true IP addr is successful set and valid
     * @return false setting up IP addr is failed and invalid
     */
    bool set_des_ip(const std::string& ip_addr);

    /**
     * @brief Accessor for pyaload size
     *
     * @return uint32_t the pyaload size in bytes
     */
    uint32_t payload_size() { return _payload_size; };

    // debug strings
    friend std::ostream& operator<<(std::ostream& os,
                                    const Message::Header& header);

   protected:
   private:
    IPAddr _source_ip;
    IPAddr _destination_ip;
    uint32_t _payload_size = 0;
  };

 private:
};
}  // namespace P2PFileSync
#endif