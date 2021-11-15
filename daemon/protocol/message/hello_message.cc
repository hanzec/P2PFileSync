#include <protolcol.pb.h>

#include "../protocol.h"

namespace P2PFileSync::Protocol {
  template <>
  void ProtocolServer::handle_packet(INSTANCE_PTR server,
                                     const PeerSession& peer_session,
                                     const ProtoHelloMessage& message){

    PeerSession new_session(server,message.public_key());



  }
}