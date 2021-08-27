#include <regex>
#include <iostream>
#include <gflags/gflags.h>
#include <utility>
#include <vector>

#include "utils/log.h"
#include "utils/ip_addr.h"

DEFINE_string(host, "", "the known host with comma-separated list");

using namespace P2PFileSync;

int main(int argc, char * argv[], const char *envp[]) {
    // init logging system
    google::InitGoogleLogging(argv[0]);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    if(gflags::GetCommandLineFlagInfoOrDie("host").is_default){
        VLOG(ERROR) << "host parameter has to be set!";
        exit(-1);
    }

    std::vector<IPAddr> known_host;
    for()
}
