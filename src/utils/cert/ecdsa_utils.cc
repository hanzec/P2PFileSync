#include <glog/logging.h>
#include <openssl/ec.h>
#include <openssl/pem.h>

#include "cert_utils.h"

namespace P2PFileSync {
template <>
std::shared_ptr<EC_KEY> generate_key_pair(const std::string &pub_key_location, 
                                          const std::string &priv_key_location) {
  auto *ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);

  // generating public/private key pair
  if ((EC_KEY_generate_key(ec_key) & EC_KEY_check_key(ec_key)) != 1) {
    LOG(ERROR) << "failed generating key pair";
    return {nullptr};
  }

  // write public key to disk
  {
    FILE *f = fopen(pub_key_location.c_str(), "w");
    PEM_write_EC_PUBKEY(f, ec_key);
    fclose(f);
  }

  // write private key to disk
  {
    FILE *f = fopen(priv_key_location.c_str(), "w");
    PEM_write_ECPrivateKey(f, ec_key, NULL, NULL, 0, NULL, NULL);
    fclose(f);
  }
  return {ec_key, EC_KEY_free};
}

template <>
std::array<std::byte, 32> generate_sha256_hash(const char* data_,
                                               const size_t& data_size,
<<<<<<< HEAD:src/utils/cert/ecdsa_utils.cc
                                               const std::shared_ptr<EC_KEY> signed_certific){
=======
                                               const std::shared_ptr<EC_KEY> signed_certificat){
>>>>>>> 2324dc188257b578aef8bfe5360e5d0fe5a0dcde:src/utils/cert/cert_utils_ecdsa.cc
  if(ECDSA)
                                              
}
}  // namespace P2PFileSync