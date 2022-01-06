###############################################################################
## Generate Protobuf ##########################################################
###############################################################################
# add dummy target for protobuf
add_library(p2p_protocol)

# protobuf deps
set(Protobuf_DEBUG ON)
set(Protobuf_USE_STATIC_LIBS ON)
find_package(Protobuf CONFIG REQUIRED)
target_link_libraries(p2p_protocol ${Protobuf_LIBRARIES})
target_include_directories(p2p_protocol PRIVATE ${Protobuf_INCLUDE_DIRS})

# generate actually files
protobuf_generate(
        LANGUAGE cpp
        TARGET p2p_protocol
        PROTOS protocol.proto)

# link protobuf_protocol to current target
target_link_libraries(daemon p2p_protocol)
target_include_directories(daemon PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

###############################################################################
## Message Handler ############################################################
###############################################################################
file(GLOB MESSAGE_HANDLER_SRCS ${CMAKE_CURRENT_LIST_DIR}/handler/*.tcc)

# add to target source
target_sources(daemon PRIVATE ${MESSAGE_HANDLER_SRCS})

# add compiler definitions for handler size
list(LENGTH MESSAGE_HANDLER_SRCS MESSAGE_HANDLER_SRCS_SIZE)
add_compile_definitions(PACKET_HANDLER_SIZE=${MESSAGE_HANDLER_SRCS_SIZE})

###############################################################################
## Package Tasks ##############################################################
###############################################################################
target_sources(daemon
        PRIVATE
        "handler_impl.h"
        "packet_utils.h"
        "packet_handler.cc"
        "packet_tasks.cc" "packet_tasks.h" )

###############################################################################
## Testing Source #############################################################
###############################################################################
#file(GLOB TEST_SRC_FILES 
#    ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)
#target_sources(p2p_protocol_test PRIVATE ${TEST_SRC_FILES})