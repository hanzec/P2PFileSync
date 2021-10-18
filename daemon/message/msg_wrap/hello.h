#ifndef P2P_FILE_SYNC_MESSAGE_MSG_WRAP_HELLO_H
#define P2P_FILE_SYNC_MESSAGE_MSG_WRAP_HELLO_H
#include <utils/ip_addr.h>
#include "warped_message.h"
#include "hello_message.pb.h"

namespace P2PFileSync {
class HelloMessage : WarpedMessage<ProtoHelloMessage>{
  

};
}  // namespace P2PFileSync
#endif // P2P_FILE_SYNC_MESSAGE_MSG_WRAP_HELLO_H