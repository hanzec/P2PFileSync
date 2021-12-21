//
// Created by hanzech on 11/27/21.
//
#include "p2p_interface.h"
#include "../command_factory.h"

namespace P2PFileSync {
class PeerCommand : public AutoRegCommand<PeerCommand> {
  REGISTER_COMMAND(PeerCommand, PEER, DEFAULT);

 public:
  void exec(std::ostringstream& out, const std::vector<std::string>& args) final {
    if (!args.empty()) {
      out << "PEER command does not need any arguments!";
    }

    auto p2p_server_ctx = P2PServerContext::get();

    out << std::setw(37) << std::left << "Peer ID" << " "
        << std::setw(20) << std::left << "Next Jump Node" << " "
        << std::setw(15) << std::left << "TTL" << std::endl;

    for (auto& peer : p2p_server_ctx->get_online_peers()){
      out << std::setw(37) << std::left << peer.first << " "
          << std::setw(20) << std::left << *peer.second.first << " "
          << std::setw(15) << std::left << peer.second.second << std::endl;
    }
  };
};
}  // namespace P2PFileSync