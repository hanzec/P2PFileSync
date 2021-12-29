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
    if (!_handler_impl<ToPayloadType<ProtoPayloadType::HELLO>>::handle(proto_msg,
                                                                       incoming_connection)) {
      LOG(ERROR) << "failed to handle proto hello message";
    } else {
      VLOG(2) << "handle proto hello message successfully";
    }
    return;
  } else {
    if () }
  switch (proto_msg.packet_type()) {
    case ProtoPayloadType::HELLO: {
      // only hello message won't include signature in outside since handshake
      ProtoHelloMessage msg;
      proto_msg.payload().UnpackTo(&msg);
      auto task = std::make_shared<std::packaged_task<void()>>(
          [proto_server, msg, signed_msg, incoming_connection]() {
            PacketHandler<ProtoHelloMessage>::handle(msg, signed_msg.prev_jump_port(),
                                                   incoming_connection, signed_msg.ttl());
          });
      proto_server->_thread_pool->submit(task);
    } break;
    case ProtoPayloadType::PING:

      break;
    case ProtoPayloadType::PONG:
      break;
    case ProtoPayloadType::ACK:
      break;
    case ProtoPayloadType::NACK:
      break;
    case ProtoPayloadType::HEARTBEAT:
      break;
    default:
      LOG(ERROR) << "unknown packet type [" << proto_msg.packet_type() << "]";
      break;
  }
  return _handler_impl<ToPayloadType<signed_msg.>>::handle(std::forward<Args>(args)...);
}

}  // namespace P2PFileSync

#include "message/hello_message.tcc"

#endif  // P2P_FILE_SYNC_PACKET_HANDLER_CC