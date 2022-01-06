#ifndef P2P_FILE_SYNC_UTILS_ECDSA_UTILS_H
#define P2P_FILE_SYNC_UTILS_ECDSA_UTILS_H

#include <openssl/bio.h>
#include <openssl/ec.h>
#include <openssl/ossl_typ.h>

#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>

namespace P2PFileSync {
/**
 * @brief generate ECDSA key pair to disk in PEM
 *
 * @param pub_key_location public PEM key location
 * @param priv_key_location private PEM key location
 * @return std::shared_ptr<EC_KEY> raw key struct, will be null if generating
 * failed
 */
template <typename Algorithm>
 std::shared_ptr<Algorithm> generate_key_pair(const std::string& pub_key_location,
                                             const std::string& priv_key_location);

/**
 * @brief Generate hash for given data and certs
 * 
 * @param data_ the raw data pointer which need to singed
 * @param data_size the size of data pointer
 * @param signed_certific the certificate for generated signed function
 * @return std::array<std::byte, 32> 
 */
template <typename Algorithm>
std::array<std::byte, 32> generate_sha256_hash(const char* data_,
                                               const size_t& data_size,
                                               std::shared_ptr<Algorithm> signed_certificat);
}  // namespace P2PFileSync

#endif