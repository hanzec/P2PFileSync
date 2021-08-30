#include "ecdsa_utils.h"

#include <glog/logging.h>
#include <openssl/ec.h>
#include <openssl/pem.h>

namespace P2PFileSync {

std::shared_ptr<EC_KEY> generate_key_pair(const std::string &pub_key_location,
                                          const std::string &priv_key_location) {
  auto *ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);

  if ((EC_KEY_generate_key(ec_key) & EC_KEY_check_key(ec_key))!= 1) {
    LOG(ERROR) << "failed generating key pair";
    return {nullptr};
  }

  {
    FILE *f = fopen(pub_key_location.c_str(), "w");
    PEM_write_EC_PUBKEY(f, ec_key);
    fclose(f);
  }

  {
    FILE *f = fopen(priv_key_location.c_str(), "w");
    PEM_write_ECPrivateKey(f, ec_key, NULL, NULL, 0, NULL, NULL);
    fclose(f);
  }
  return {ec_key, EC_KEY_free};
}
}  // namespace P2PFileSync