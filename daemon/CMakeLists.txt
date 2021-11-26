###############################################################################
## Daemon Targets #############################################################
###############################################################################
# exec target
add_executable(daemon)

# test target
add_executable(daemon_test)

###############################################################################
## Shared Dependency ##########################################################
###############################################################################
# google log
find_package(glog CONFIG REQUIRED)
target_link_libraries(daemon glog::glog)
target_include_directories(daemon PRIVATE ${glog_INCLUDE_DIRS})

# google flag
find_package(gflags CONFIG REQUIRED)
target_link_libraries(daemon gflags_static)
target_include_directories(daemon PRIVATE ${GFLAGS_INCLUDE_DIR})

# protobuf
find_package(Protobuf CONFIG REQUIRED)
target_link_libraries(daemon protobuf::libprotobuf)
target_include_directories(daemon PRIVATE ${Protobuf_INCLUDE_DIRS})

# openssl
find_package(OpenSSL REQUIRED)
target_link_libraries(daemon OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(daemon PRIVATE ${OPENSSL_INCLUDE_DIR})

# yaml-cpp
find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(daemon ${YAML_CPP_LIBRARIES})
target_include_directories(daemon PRIVATE ${YAML_CPP_INCLUDE_DIR})

# libevent
find_package(Libevent CONFIG REQUIRED)
target_link_libraries(daemon libevent::core libevent::extra )
target_include_directories(daemon PRIVATE ${Libevent_CORE_INCLUDE_DIRS})

# uuid utils
find_path(LIBUUID_INCLUDE_DIR uuid.h PATH_SUFFIXES uuid)
find_library(LIBUUID_LIBRARY libuuid.a)
target_link_libraries(daemon_test ${LIBUUID_LIBRARY})
target_include_directories(daemon_test PRIVATE ${LIBUUID_INCLUDE_DIR})

# google test deps(require pthread)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(daemon_test Threads::Threads)
target_link_libraries(daemon_test ${GTEST_LIBRARY})
target_include_directories(daemon_test PRIVATE ${GTEST_INCLUDE_DIRS})

###############################################################################
## Compile Config #############################################################
###############################################################################
# compile under cxx 17 since need std::byte support
target_compile_features(daemon PRIVATE cxx_std_17)
target_compile_features(daemon_test PRIVATE cxx_std_17)

###############################################################################
## Commands ###################################################################
###############################################################################
add_subdirectory(interface)

###############################################################################
## Project Utils ##############################################################
###############################################################################
add_subdirectory(utils)

###############################################################################
## Message Packet ##############################################################
###############################################################################
add_subdirectory(packet)

###############################################################################
## Server Protocol Kit ########################################################
###############################################################################
add_subdirectory(server_kit)
target_link_libraries(daemon server_connection)
target_include_directories(daemon PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/server_kit/server_kit.h")
target_include_directories(daemon PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/server_kit/model/response")

###############################################################################
## Project Source #############################################################
###############################################################################
target_sources(daemon
        PRIVATE
        "main.cc" "server_kit/export.h" "common.h"
        "manage_interface.cc" "manage_interface.h"
        "peer_session.cc" "protocol_server.cc" "p2p_interface.h")
target_include_directories(daemon PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

###############################################################################
## Testing ####################################################################
###############################################################################
# test main
target_sources(daemon_test PRIVATE "test_main.cc")

# scan for possibale test
gtest_discover_tests(daemon_test)