#include "../protocol.h"
#include "hello_message.pb.h"

namespace P2PFileSync::Protocol {
  template <>
  void ProtocolServer::handle_packet(ProtocolServer& server,
                            const PeerSession& peer_session, const ProtoHelloMessage&& message){
  
  }
}