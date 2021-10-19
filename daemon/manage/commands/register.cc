#include "command.h"
#include "daemon_state.h"
#include "server_kit/server_kit.h"
#include "utils/status.h"

NEW_COMMAND(REGISTER, "get current time of system", output, args, daemon_status){

  if(!daemon_status.get_register_status()){
    if(args.size() == 1){
      if(daemon_status.exist_option("CLIENT_REGISTER_PIN")){
        if(std::stoi(args[0]) == daemon_status.get_option<int>("CLIENT_REGISTER_PIN")){
          daemon_status.set_register_status(true);
          output << "client successful registered";
        }
      }else{
        output << "Error: you should run REGISTER without argument first";
      }
    }else {
      output << "Error: [REGISTER] will only take at most 1 arguments";
    }
    auto ret = P2PFileSync_SK_register_client();

    if(!P2PFileSync_SK_success(ret)){
      output << "failed to register new client to management server!";
    }else{
      int pass_code = std::stoi(P2PFileSync_SK_get_data(ret));
      daemon_status.store_option("CLIENT_REGISTER_PIN", pass_code);
      output << "regist request sent, please visit [" << P2PFileSync.
    }
  }else{
    output << "client is already registered!";
  }
}
