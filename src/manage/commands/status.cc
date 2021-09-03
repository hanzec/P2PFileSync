#include "command.h"

namespace P2PFileSync::AUTO_GEN_COMMAND {                                   
  class STATUS {                                                             
   public:                                                                    
    STATUS() { (void)_reg_status; };                                         
    static void do_operation(std::ostringstream& output,                      
                             const std::vector<std::string>& args);           
                                                                              
   private:                                                                   
    friend class P2PFileSync::CommandFactory;
    const static P2PFileSync::Status _reg_status = P2PFileSync::CommandFactory::register_object( 
        "STATUS", "123", &STATUS::do_operation);          
  };                                                                          
  };                                                                          
  void P2PFileSync::AUTO_GEN_COMMAND::STATUS::do_operation(                  
      const std::vector<std::string>& args, std::ostringstream& output){}
