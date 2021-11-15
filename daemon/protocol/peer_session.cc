#include <openssl/evp.h>

#include "protocol.h"

namespace P2PFileSync::Protocol {
std::shared_ptr<ProtocolServer::PeerSession> ProtocolServer::PeerSession::new_session(const PKCS7 * cert) {

  return std::shared_ptr<PeerSession>();
}


bool ProtocolServer::PeerSession::verify(const std::string& data, const std::string& sig,
                                         const std::string& method) {
  EVP_PKEY* pubKey = EVP_PKEY_new();
  EVP_PKEY_assign_EC_KEY(pubKey, eckey);
  EVP_MD_CTX* m_RSAVerifyCtx = EVP_MD_CTX_create();
}
}  // namespace P2PFileSync::Protocol