#ifndef P2P_FILE_SYNC_MESSAGE_MESSAGE_H
#define P2P_FILE_SYNC_MESSAGE_MESSAGE_H

#include <utils/ip_addr.h>

#include "payload.pb.h"

namespace P2PFileSync {
class AbstractPayload {
 public:
  Message();

  template<typename payload_type>
  bool is(){return _payload_message.payload().Is<payload_type>();}
 private : 
  ProtoRequestPayload _payload_message;
};
}  // namespace P2PFileSync
#endif