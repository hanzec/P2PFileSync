###############################################################################
## Generate Protobuf ##########################################################
###############################################################################
message(STATUS PROTO_SRC_FILES)

# generate actually files
EXEC_PROGRAM(${PROTOBUF_PROTOC_EXECUTABLE} ARGS
        --proto_path "${CMAKE_CURRENT_LIST_DIR}"
        --cpp_out ${CMAKE_CURRENT_BINARY_DIR} "${CMAKE_CURRENT_LIST_DIR}/protocol.proto" RETURN_VALUE PROTO_BUILD_STATE)

if (PROTO_BUILD_STATE AND NOT PROTO_BUILD_STATE EQUAL 0)
    message(FATAL_ERROR "Build protobuf failed!")
endif (PROTO_BUILD_STATE AND NOT PROTO_BUILD_STATE EQUAL 0)

# protobuf generated cpp source
target_sources(daemon PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/protocol.pb.cc")

# include protobuf generated folders
target_include_directories(daemon PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

###############################################################################
## Message Handler ############################################################
###############################################################################
file(GLOB MESSAGE_HANDLER_SRCS ${CMAKE_CURRENT_LIST_DIR}/message/*.tcc)

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
        "packet_utils.h"
        "packet_handler.cc"
        "packet_tasks.cc" "packet_tasks.h" )

###############################################################################
## Testing Source #############################################################
###############################################################################
#file(GLOB TEST_SRC_FILES 
#    ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)
#target_sources(p2p_protocol_test PRIVATE ${TEST_SRC_FILES})