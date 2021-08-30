#ifndef P2P_FILE_SYNC_UTILS_ECDSA_UTILS_H
#define P2P_FILE_SYNC_UTILS_ECDSA_UTILS_H

#include <memory>
#include <string>
#include <utility>
#include <openssl/bio.h>
#include <openssl/ec.h>

namespace P2PFileSync {
    /**
     * @brief generate ECDSA key pair to disk in PEM
     * 
     * @param pub_key_location public PEM key loocation
     * @param priv_key_location private PEM key loocation
     * @return std::shared_ptr<EC_KEY> raw key struct, will be null if geneateing failed
     */
    std::shared_ptr<EC_KEY> generate_key_pair(std::string & pub_key_location, 
                                              std::string & priv_key_location);
} // P2PFileSync

#endif