#ifndef P2P_FILE_SYNC_MANAGE_MANAGE_H
#define P2P_FILE_SYNC_MANAGE_MANAGE_H
#include <sys/socket.h>

#include <array>
#include <string>

#include "../utils/ip_addr.h"

extern "C" {
enum __attribute__((__packed__)) listen_type {
  /**
   * listen tcp sockets
   */
  TCP = AF_INET,

  /**
   * listen file_discriptors
   */
  SOCKET_FILE = AF_UNIX,
};

/**
 * @brief generate human-readable error message from error number
 *
 * @param error_no the error number return by function
 * @param output_size the generate message from
 * @return char*
 */
char* P2PFileSync_get_error_msg(int error_no, int* output_size);

/**
 * @brief Start manage interface for P2PFileSync with socket address
 *
 * @param ip_address the address and port need to listen
 */
int P2PFileSync_start_manage_thread(const char* socket, listen_type listen_type);
}
#endif
