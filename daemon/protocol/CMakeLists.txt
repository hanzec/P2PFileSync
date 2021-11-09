###############################################################################
## Message Wraps ##############################################################
###############################################################################
file(GLOB MESSAGE_WRAP_SRC_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/msg_wrap/*.h)
target_sources(daemon PRIVATE ${MESSAGE_WRAP_SRC_FILES})

###############################################################################
## Generate Protobuf ##########################################################
###############################################################################
# collecting all proto files
file(GLOB_RECURSE PROTO_SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/proto/*.proto")

message(STATUS PROTO_SRC_FILES)

# generate actually files
EXEC_PROGRAM(${PROTOBUF_PROTOC_EXECUTABLE} ARGS 
    --proto_path "${CMAKE_CURRENT_LIST_DIR}/proto"
    --cpp_out ${CMAKE_CURRENT_BINARY_DIR} ${PROTO_SRC_FILES} RETURN_VALUE PROTO_BUILD_STATE)

if(PROTO_BUILD_STATE AND NOT PROTO_BUILD_STATE EQUAL 0)
    message(FATAL_ERROR "Build protobuf failed!")
endif(PROTO_BUILD_STATE AND NOT PROTO_BUILD_STATE EQUAL 0)

# include protobuf generated folders
target_include_directories(daemon PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

###############################################################################
## Project Source #############################################################
###############################################################################
set_target_properties(daemon PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "protocol.h")

target_sources(daemon 
    PRIVATE
    "message.h" "message.cc"
    "protocol.h" "protocol_server.cc" "peer_session.cc"
)
target_include_directories(daemon PRIVATE ${PROJECT_SOURCE_DIR}/daemon/utils/)

###############################################################################
## Testing Source #############################################################
###############################################################################
#file(GLOB TEST_SRC_FILES 
#    ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)
#target_sources(p2p_protocol_test PRIVATE ${TEST_SRC_FILES})