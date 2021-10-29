#ifndef P2P_FILE_SYNC_MESSAGE_MSG_WRAP_WARPED_MESSAGE_H
#define P2P_FILE_SYNC_MESSAGE_MSG_WRAP_WARPED_MESSAGE_H

namespace P2PFileSync {
template <typename proto_class>
class WarpedMessage{
public:
  WarpedMessage(proto_class input_proto):_proto_defs(input_proto){};

private:
  proto_class _proto_defs;

protected:
 /**
  * @brief Get the proto class object
  * 
  * @return proto_class 
  */
  proto_class get_proto_class() {return _proto_defs;}

};

}  // namespace P2PFileSync
#endif // P2P_FILE_SYNC_MESSAGE_MSG_WRAP_WARPED_MESSAGE_H