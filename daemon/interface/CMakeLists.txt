###############################################################################
## Commands ###################################################################
###############################################################################
# commans sources
file(GLOB COMMAND_SRCS command/*.cc)
target_sources(daemon PRIVATE ${COMMAND_SRCS})

###############################################################################
## Sources ####################################################################
###############################################################################
# project sources
target_sources(daemon 
    PRIVATE
    ${COMMAND_SRCS}
    "command_executor.cc" "command_executor.h"
    "connection_session.cc" "connection_session.h"
)