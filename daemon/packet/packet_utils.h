//
// Created by hanzech on 12/17/21.
//

#ifndef P2P_FILE_SYNC_PACKET_UTILS_CC
#define P2P_FILE_SYNC_PACKET_UTILS_CC
#include "protocol.pb.h"

namespace P2PFileSync {
template <ProtoPayloadType Value>
struct ToPayloadType {
  typedef void type;
  enum { value = false };
};
#define DEFINE_ENUMERATED_TYPE(TYPE, ENUM) \
  template <>                              \
  struct ToPayloadType<ENUM> {             \
    typedef TYPE type;                     \
    enum { value = true };                 \
  }

DEFINE_ENUMERATED_TYPE(ProtoHelloMessage, ProtoPayloadType::HELLO);
}  // namespace P2PFileSync

#endif  // P2P_FILE_SYNC_PACKET_UTILS_CC
