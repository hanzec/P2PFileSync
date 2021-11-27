#include <openssl/x509_vfy.h>
#include <protocol.pb.h>

#include "p2p_interface.h"
namespace P2PFileSync {
template <>
class P2PServerContext::IMessageHandler<ProtoHelloMessage> {
 public:
  static bool handle_complicated(const std::shared_ptr<P2PServerContext>& server, const ProtoHelloMessage& message,
                     struct sockaddr_in *incoming_connection, int ttl){
    auto session_ret = server->_peer_pool.get_instance(message.sender_id());

    // update client session
    if (session_ret == nullptr) {
      auto new_session = P2PServerContext::PeerSession::new_session(
          message.x509_certificate(), X509_STORE_CTX_get_chain(server->_x509_store_ctx));

      // during creation of the peer session will verify the client cert from server certificate
      if(new_session == nullptr){
        LOG(ERROR) << "failed to verify certificate by server public certificate chain for client[" << message.sender_id() << "]";
        return false;
      }

      // TODO need to modify [SHA256] later avoid magic variables
      if (new_session->verify(message.sender_id(), message.sender_id(), "SHA256")) {
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
    IPAddr dest = IPAddr(incoming_connection,message.port());
    if(server->can_delivered(message.sender_id())){
      if(server->try_update_table(message.sender_id(), dest, ttl)){
        LOG(INFO) << "route to peer [" << message.sender_id() << "] is update! ";
      }
    } else {
      server->add_new_route(message.sender_id(), dest, ttl);
      LOG(INFO) << "adding new route of peer [" << message.sender_id() << "] to routing table! ";
    }
    return true;
  }
};

}  // namespace P2PFileSync::Protocol