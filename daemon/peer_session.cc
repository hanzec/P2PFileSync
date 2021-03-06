#include <openssl/err.h>
#include <openssl/evp.h>

#include <utility>

#include "p2p_interface.h"

namespace P2PFileSync {

const constexpr char* get_validation_errstr(long e) {
  switch ((int)e) {
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
      return "ERR_UNABLE_TO_GET_ISSUER_CERT";
    case X509_V_ERR_UNABLE_TO_GET_CRL:
      return "ERR_UNABLE_TO_GET_CRL";
    case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE:
      return "ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE";
    case X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE:
      return "ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE";
    case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY:
      return "ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY";
    case X509_V_ERR_CERT_SIGNATURE_FAILURE:
      return "ERR_CERT_SIGNATURE_FAILURE";
    case X509_V_ERR_CRL_SIGNATURE_FAILURE:
      return "ERR_CRL_SIGNATURE_FAILURE";
    case X509_V_ERR_CERT_NOT_YET_VALID:
      return "ERR_CERT_NOT_YET_VALID";
    case X509_V_ERR_CERT_HAS_EXPIRED:
      return "ERR_CERT_HAS_EXPIRED";
    case X509_V_ERR_CRL_NOT_YET_VALID:
      return "ERR_CRL_NOT_YET_VALID";
    case X509_V_ERR_CRL_HAS_EXPIRED:
      return "ERR_CRL_HAS_EXPIRED";
    case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
      return "ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD";
    case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
      return "ERR_ERROR_IN_CERT_NOT_AFTER_FIELD";
    case X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD:
      return "ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD";
    case X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD:
      return "ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD";
    case X509_V_ERR_OUT_OF_MEM:
      return "ERR_OUT_OF_MEM";
    case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
      return "ERR_DEPTH_ZERO_SELF_SIGNED_CERT";
    case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
      return "ERR_SELF_SIGNED_CERT_IN_CHAIN";
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
      return "ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY";
    case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
      return "ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE";
    case X509_V_ERR_CERT_CHAIN_TOO_LONG:
      return "ERR_CERT_CHAIN_TOO_LONG";
    case X509_V_ERR_CERT_REVOKED:
      return "ERR_CERT_REVOKED";
    case X509_V_ERR_INVALID_CA:
      return "ERR_INVALID_CA";
    case X509_V_ERR_PATH_LENGTH_EXCEEDED:
      return "ERR_PATH_LENGTH_EXCEEDED";
    case X509_V_ERR_INVALID_PURPOSE:
      return "ERR_INVALID_PURPOSE";
    case X509_V_ERR_CERT_UNTRUSTED:
      return "ERR_CERT_UNTRUSTED";
    case X509_V_ERR_CERT_REJECTED:
      return "ERR_CERT_REJECTED";
    case X509_V_ERR_SUBJECT_ISSUER_MISMATCH:
      return "ERR_SUBJECT_ISSUER_MISMATCH";
    case X509_V_ERR_AKID_SKID_MISMATCH:
      return "ERR_AKID_SKID_MISMATCH";
    case X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH:
      return "ERR_AKID_ISSUER_SERIAL_MISMATCH";
    case X509_V_ERR_KEYUSAGE_NO_CERTSIGN:
      return "ERR_KEYUSAGE_NO_CERTSIGN";
    case X509_V_ERR_INVALID_EXTENSION:
      return "ERR_INVALID_EXTENSION";
    case X509_V_ERR_INVALID_POLICY_EXTENSION:
      return "ERR_INVALID_POLICY_EXTENSION";
    case X509_V_ERR_NO_EXPLICIT_POLICY:
      return "ERR_NO_EXPLICIT_POLICY";
    case X509_V_ERR_APPLICATION_VERIFICATION:
      return "ERR_APPLICATION_VERIFICATION";
    default:
      return "ERR_UNKNOWN";
  }
}

std::shared_ptr<P2PServerContext::PeerSession> P2PServerContext::PeerSession::new_session(
    const std::string& raw_cert) noexcept {
  X509* cert;
  std::shared_ptr<P2PServerContext::PeerSession> ret = nullptr;
  const auto* data = reinterpret_cast<const unsigned char*>(raw_cert.c_str());

  // construct X509 Certificate from raw
  if (nullptr == (cert = d2i_X509(nullptr, &data, raw_cert.size()))) {
    LOG(ERROR) << "failed to unpack the device's X509 certificate";
    return nullptr;
  }

  // verify certificate
  X509_STORE_CTX* x509_store_ctx = X509_STORE_CTX_new();
  X509_STORE_CTX_init(x509_store_ctx, get()->_x509_store, cert, nullptr);

  if (X509_verify_cert(x509_store_ctx) == 1) {
    ret = std::shared_ptr<P2PServerContext::PeerSession>(
        new P2PServerContext::PeerSession(X509_get_pubkey(cert)));
  } else {
    LOG(ERROR) << get_validation_errstr(X509_STORE_CTX_get_error(x509_store_ctx));
  }

  X509_free(cert);
  X509_STORE_CTX_free(x509_store_ctx);
  return ret;
}

P2PServerContext::PeerSession::~PeerSession() {
  EVP_PKEY_free(_public_key);
  if (_evp_md_ctx != nullptr) EVP_MD_CTX_free(_evp_md_ctx);
}

P2PServerContext::PeerSession::PeerSession(EVP_PKEY* _public_key) noexcept
    : _public_key(_public_key) {
  LOG(INFO) << "create peer instance";
}

// TODO modify here using protobuf enum of digest Algorithm
bool P2PServerContext::PeerSession::verify(const std::string& data, const std::string& sig,
                                           const ProtoSignAlgorithm& method) noexcept {
  // creating ctx if not initialized
  if (_evp_md_ctx == nullptr) {
    if ((_evp_md_ctx = EVP_MD_CTX_new()) == nullptr) {
      LOG(ERROR) << "EVP_MD_CTX_create failed, error" << std::hex << ERR_get_error();
      return false;
    }

    // looking for encryption method
    const EVP_MD* algorithm = nullptr;
    switch (method) {
      case SHA_256:
        algorithm = EVP_sha256();
        break;
      default:
        LOG(ERROR) << "unsupported digest algorithm";
        EVP_MD_CTX_free(_evp_md_ctx);
        return false;
    }

    // setting up encryption context
    if (1 != EVP_DigestVerifyInit(_evp_md_ctx, nullptr, algorithm, nullptr, _public_key)) {
      LOG(ERROR) << "EVP_DigestVerifyInit failed, error" << std::hex << ERR_get_error();
      return false;
    }
  }

  // verify the sig
  if (1 != EVP_DigestVerifyUpdate(_evp_md_ctx, data.c_str(), data.length())) {
    LOG(ERROR) << "EVP_DigestVerifyUpdate failed, error" << std::hex << ERR_get_error();
    return false;
  }

  return 1 == EVP_DigestVerifyFinal(_evp_md_ctx,
                                    reinterpret_cast<const unsigned char*>(sig.c_str()),
                                    sig.length());
}

}  // namespace P2PFileSync