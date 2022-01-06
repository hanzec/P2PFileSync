//
// Created by 陈瀚泽 on 2021/12/27.
//

#ifndef P2P_FILE_SYNC_PACKET_HANDLER_CC
#define P2P_FILE_SYNC_PACKET_HANDLER_CC
#include <netinet/in.h>
#include <openssl/x509.h>
#include <protocol.pb.h>

#include <functional>

#include "../p2p_interface.h"
#include "packet_tasks.h"
#include "packet_utils.h"

namespace P2PFileSync {
void P2PServerContext::PacketHandler::handle(SignedProtoMessage& signed_msg,
                                             const sockaddr_in* incoming_connection) {
  // load message
  ProtoMessage proto_msg;
  proto_msg.ParseFromString(signed_msg.signed_payload());

  /* proto hello message is a special case since need to use hello package to set up init
  routing and encryption node */
  if (proto_msg.packet_type() == ProtoPayloadType::HELLO) {
    if (!_handler_impl<ProtoHelloMessage>::handle(proto_msg, incoming_connection)) {
      VLOG(1) << "failed to handle proto hello message";
    } else {
      VLOG(2) << "handle proto hello message successfully";
    }
    return;
  } else {
    // check current message is already verified or not
    auto checker = get()->_peer_pool.get_instance(proto_msg.sender());
    if (checker == nullptr) {
      VLOG(1) << "peer [" << proto_msg.sender() << "] not found";
      return;
    }

    // verify message
    auto data = proto_msg.payload().SerializeAsString();
    if (checker->verify(data, signed_msg.signature(), signed_msg.sign_algorithm())) {
      // handle message
      switch (proto_msg.packet_type()) {
        default:
          VLOG(1) << "unknown packet type [" << proto_msg.packet_type() << "]";
          break;
      }
    } else {
      VLOG(1) << "failed to verify packet [" << signed_msg.signature()
              << "] from peer: " << proto_msg.sender();
      return;
    }
  }
}

}  // namespace P2PFileSync

#endif  // P2P_FILE_SYNC_PACKET_HANDLER_CC