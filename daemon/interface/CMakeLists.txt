###############################################################################
## Sources ####################################################################
###############################################################################
# project sources
target_sources(p2p_file_sync 
    PRIVATE
    ${COMMAND_SRCS}
    "command_executor.cc" "command_executor.h"
    "connection_session.cc" "connection_session.h"
)

# commans sources
file(GLOB COMMAND_SRCS command/*.cc)
target_sources(p2p_file_sync PRIVATE ${COMMAND_SRCS})