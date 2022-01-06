//
// Created by hanzech on 12/1/21.
//

#ifndef P2P_FILE_SYNC_RABIN_CHECKSUM_H
#define P2P_FILE_SYNC_RABIN_CHECKSUM_H
#include <string>

/**
 * This class implements the Rabin fingerprinting algorithm rewrite in C++.
 *  ref:
 *      http://www.xmailserver.org/rabin.pdf
 *      https://en.wikipedia.org/wiki/Rabin%E2%80%93Karp_algorithm
 *  code_base: https://github.com/baixiangcpp/FileCDC/blob/master/checksum.h
 */
namespace P2PFileSync::RabinChecksum {

/**
 * Generate a Rabin fingerprint for a given string.
 * @param buf a raw (human-unreadable) string containthe data to be fingerprinted.
 * @return the fingerprint of the given string.
 */
uint64_t checksum(std::string& buf);

/**
 *  Generate a Rabin fingerprint from existing fingerprint by moving window to next byte.
 * @param prev_sum the previous fingerprint.
 * @param len fingerprint's data length.
 * @param out bytes move out of the window.
 * @param in bytes move in to the window.
 * @return
 */
uint64_t rolling_checksum(uint64_t prev_sum, std::byte out, std::byte in);
}
#endif  // P2P_FILE_SYNC_RABIN_CHECKSUM_H
