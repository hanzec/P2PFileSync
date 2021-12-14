#include <openssl/x509_vfy.h>
#include <protocol.pb.h>

#include "p2p_interface.h"
namespace P2PFileSync {
template <>
ProtoHelloMessage P2PServerContext::construct_payload_internal() {
  unsigned char* raw_cert = nullptr;
  if (0 > i2d_X509(_client_cert, &raw_cert)) {
    LOG(ERROR) << "unable to get raw certificate from x509 store!";
  }

  // fill in proto hello message
  ProtoHelloMessage hello_message;
  hello_message.set_sender_id(_device_context->device_id());
  hello_message.set_x509_certificate(raw_cert, i2d_X509(_client_cert, nullptr));
  free(raw_cert);
  return hello_message;
}

template <>
bool P2PServerContext::handle_complicated(const std::shared_ptr<P2PServerContext>& server,
                                          const ProtoHelloMessage& message,
                                          struct sockaddr_in* incoming_connection,
                                          uint32_t ttl) {
  auto session_ret = server->_peer_pool.get_instance(message.sender_id());

  // update client session
  if (session_ret == nullptr) {
    auto new_session = P2PServerContext::PeerSession::new_session(message.x509_certificate());

    // during creation of the peer session will verify the client cert from server certificate
    if (new_session == nullptr) {
      LOG(ERROR) << "failed to load verify session of the node [" << message.sender_id()
                 << "]";
      return false;
    }

    // todo add verification of the client certificate
    // TODO need to modify [SHA256] later avoid magic variables
//    if (new_session->verify(message.sender_id(), message.sender_id(), "SHA256")) {
    if (true) {
      // add client to server endpoint
      server->_peer_pool.add_instance(message.sender_id(), new_session);
      LOG(INFO) << "successful add client [" << std::hex << message.sender_id() << "]";
    } else {
      LOG(WARNING) << "failed add client [" << std::hex << message.sender_id()
                   << "] since signature verification failed";
      return false;
    }
  }

  // update routing table if possible
  IPAddr dest = IPAddr(incoming_connection, message.port());
  if (server->can_delivered(message.sender_id())) {
    if (server->try_update_table(message.sender_id(), dest, ttl)) {
      LOG(INFO) << "route to peer [" << message.sender_id() << "] is update! ";
    }
  } else {
    server->add_new_route(message.sender_id(), dest, ttl);
    LOG(INFO) << "adding new route of peer [" << message.sender_id() << "] to routing table! ";
  }
  return true;
}

}  // namespace P2PFileSync