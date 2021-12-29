#include "../packet_handler.cc"
#include "p2p_interface.h"

#include <glog/logging.h>
#include <openssl/x509_vfy.h>
namespace P2PFileSync {
static ProtoHelloMessage construct_internal(X509* cert) noexcept {
  unsigned char* raw_cert = nullptr;
  if (0 > i2d_X509(cert, &raw_cert)) {
    LOG(ERROR) << "unable to get raw certificate from x509 store!";
  }

  // fill in proto hello message
  ProtoHelloMessage hello_message;
  hello_message.set_sender_id(P2PServerContext::_device_context->device_id());
  hello_message.set_x509_certificate(raw_cert, i2d_X509(cert, nullptr));
  free(raw_cert);
  VLOG(3) << "constructing hello message ";
  return hello_message;
}

template<> bool PacketUtils::__handler_impl<ProtoHelloMessage>::handler_internal(const ProtoHelloMessage& message, uint16_t prev_port,
                                                                      const sockaddr_in* client_address, uint32_t ttl) noexcept {
  auto server = P2PServerContext::get();
  auto session_ret = server->_peer_pool.get_instance(message.sender_id());

  // update client session
  if (session_ret == nullptr) {
    auto new_session =
        P2PServerContext::PeerSession::new_session(message.x509_certificate());

    // during creation of the peer session will verify the client cert from server
    // certificate
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
  auto new_ip = std::make_shared<IPAddr>(client_address, prev_port);
  auto dest = server->_peer_pool.get_instance(message.sender_id());
  if (server->can_delivered(message.sender_id())) {
    if (server->try_update_table(message.sender_id(), new_ip, ttl)) {
      LOG(INFO) << "route to peer [" << message.sender_id() << "] is update! ";
    }

    // if fist time received from this peer, send hello message to it
    server->send_pkg(
        server->package_pkg<ProtoHelloMessage>(
            construct_internal(server->_client_cert), message.sender_id()),
        new_ip);
  } else {
    server->add_new_route(message.sender_id(), new_ip, ttl);
    LOG(INFO) << "adding new route of peer [" << message.sender_id()
              << "] to routing table! ";
  }
  return true;
}

}  // namespace P2PFileSync