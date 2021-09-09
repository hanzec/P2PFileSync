###############################################################################
## Fuse Implementation  #######################################################
###############################################################################
add_subdirectory(fuse)

###############################################################################
## Project Utils ##############################################################
###############################################################################
add_subdirectory(utils)

###############################################################################
## Manage Interface ###########################################################
###############################################################################
add_subdirectory(manage)

###############################################################################
## Message Packet ##############################################################
###############################################################################
add_subdirectory(message)

###############################################################################
## Generate Protobuf ##########################################################
###############################################################################
# collecting all proto files
file(GLOB_RECURSE PROTO_SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/proto/*.proto")

# generate actually files
EXEC_PROGRAM(${PROTOBUF_PROTOC_EXECUTABLE} ARGS 
    --proto_path "${CMAKE_CURRENT_LIST_DIR}/proto"
    --cpp_out ${CMAKE_CURRENT_BINARY_DIR} ${PROTO_SRC_FILES} RETURN_VALUE PROTO_BUILD_STATE)

if(PROTO_BUILD_STATE AND NOT PROTO_BUILD_STATE EQUAL 0)
    message(FATAL_ERROR "Build protobuf failed!")
endif(PROTO_BUILD_STATE AND NOT PROTO_BUILD_STATE EQUAL 0)

# include protobuf generated folders
target_include_directories(p2p_file_sync PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

###############################################################################
## Project Source #############################################################
###############################################################################
target_sources(p2p_file_sync 
    PRIVATE
    "main.cc"
    "common.h"
    "sync.cc" "sync.h"
    PUBLIC
    ${PROTO_SRCS} ${PROTO_HDRS}
)

###############################################################################
## Testing Source #############################################################
###############################################################################
target_sources(test_target PRIVATE "test_main.cc")