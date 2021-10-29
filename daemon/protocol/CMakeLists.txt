###############################################################################
## P2P Protocol Utils #########################################################
###############################################################################
add_library(p2p_protocol)

###############################################################################
## Compile Config #############################################################
###############################################################################
# compile under cxx 17 since need std::byte support
target_compile_features(p2p_protocol PRIVATE cxx_std_17)

###############################################################################
## Dependency #################################################################
###############################################################################
# protobuf
find_package(Protobuf CONFIG REQUIRED)
target_link_libraries(p2p_protocol protobuf::libprotobuf)
target_include_directories(p2p_protocol PRIVATE ${Protobuf_INCLUDE_DIRS})

###############################################################################
## Message Wraps ##############################################################
###############################################################################
file(GLOB MESSAGE_WRAP_SRC_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/msg_wrap/*.h)
target_sources(p2p_file_sync_test PRIVATE ${MESSAGE_WRAP_SRC_FILES})

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
target_include_directories(p2p_protocol PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

###############################################################################
## Project Source #############################################################
###############################################################################
target_sources(p2p_protocol 
    PRIVATE
    "message.h"
    "message.cc"
    ${PROJECT_SOURCE_DIR}/daemon/utils/ip_addr.cc
)

target_include_directories(p2p_protocol PRIVATE ${PROJECT_SOURCE_DIR}/daemon/utils/)

###############################################################################
## Testing Source #############################################################
###############################################################################
file(GLOB TEST_SRC_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)
target_sources(p2p_file_sync_test PRIVATE ${TEST_SRC_FILES})