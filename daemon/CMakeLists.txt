###############################################################################
## Daemon Targets #############################################################
###############################################################################
# exec target
add_executable(p2p_file_sync)

# test target
add_executable(p2p_file_sync_test)

###############################################################################
## Shared Dependency ##########################################################
###############################################################################
# google log
find_package(glog CONFIG REQUIRED)
target_link_libraries(p2p_file_sync glog::glog)
target_include_directories(p2p_file_sync PRIVATE ${glog_INCLUDE_DIRS})

# google flag
find_package(gflags CONFIG REQUIRED)
target_link_libraries(p2p_file_sync gflags_static)
target_include_directories(p2p_file_sync PRIVATE ${GFLAGS_INCLUDE_DIR})

# protobuf
find_package(Protobuf CONFIG REQUIRED)
target_link_libraries(p2p_file_sync protobuf::libprotobuf)
target_include_directories(p2p_file_sync PRIVATE ${Protobuf_INCLUDE_DIRS})

# openssl
find_package(OpenSSL REQUIRED)
target_link_libraries(p2p_file_sync OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(p2p_file_sync PRIVATE ${OPENSSL_INCLUDE_DIR})

# yaml-cpp
find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(p2p_file_sync ${YAML_CPP_LIBRARIES})
target_include_directories(p2p_file_sync PRIVATE ${YAML_CPP_INCLUDE_DIR})

# google test deps(require pthread)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(p2p_file_sync_test Threads::Threads)
target_link_libraries(p2p_file_sync_test ${GTEST_LIBRARY})
target_include_directories(p2p_file_sync_test PRIVATE ${GTEST_INCLUDE_DIRS})

###############################################################################
## Compile Config #############################################################
###############################################################################
# compile under cxx 17 since need std::byte support
target_compile_features(p2p_file_sync PRIVATE cxx_std_17)
target_compile_features(p2p_file_sync_test PRIVATE cxx_std_17)

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
## Server Protocol Kit ########################################################
###############################################################################
add_subdirectory(server_kit)
target_link_libraries(p2p_file_sync server_connection)
target_include_directories(p2p_file_sync PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/server_kit/server_kit.h")

###############################################################################
## Project Source #############################################################
###############################################################################
target_sources(p2p_file_sync 
    PRIVATE
    "main.cc"
    "common.h"
    PUBLIC
    ${PROTO_SRCS} ${PROTO_HDRS}
)

target_include_directories(p2p_file_sync PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
###############################################################################
## Testing ####################################################################
###############################################################################
# test main
target_sources(p2p_file_sync_test PRIVATE "test_main.cc")

# scan for possibale test
gtest_discover_tests(p2p_file_sync_test)