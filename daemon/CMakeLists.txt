include(utils)

###############################################################################
## Daemon Targets #############################################################
###############################################################################
# exec target
add_executable(daemon)

# test target
add_executable(daemon_test)

###############################################################################
## Project Versions ###########################################################
###############################################################################
# generate build time
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")

# collect head commit hash
set(GIT_HASH "")
get_git_hash(GIT_HASH)

# generate the configuration file
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.h)
target_include_directories(daemon PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

###############################################################################
## Shared Dependency ##########################################################
###############################################################################
# google log
find_package(glog CONFIG REQUIRED)
target_link_libraries(daemon glog::glog)
target_link_libraries(daemon_test glog::glog)
target_include_directories(daemon PRIVATE ${glog_INCLUDE_DIRS})

# google flag
find_package(gflags CONFIG REQUIRED)
target_link_libraries(daemon gflags_static)
target_include_directories(daemon PRIVATE ${GFLAGS_INCLUDE_DIR})

# openssl
find_package(OpenSSL REQUIRED)
target_link_libraries(daemon OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(daemon PRIVATE ${OPENSSL_INCLUDE_DIR})

# yaml-cpp
find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(daemon ${YAML_CPP_LIBRARIES})
target_link_libraries(daemon_test ${YAML_CPP_LIBRARIES})
target_include_directories(daemon PRIVATE ${YAML_CPP_INCLUDE_DIR})

# libevent
find_package(Libevent CONFIG REQUIRED)
target_link_libraries(daemon libevent::core libevent::extra )
target_include_directories(daemon PRIVATE ${Libevent_CORE_INCLUDE_DIRS})

# uuid utils
find_path(LIBUUID_INCLUDE_DIR uuid.h PATH_SUFFIXES uuid)
find_library(LIBUUID_LIBRARY libuuid.a)
target_link_libraries(daemon ${LIBUUID_LIBRARY})
target_link_libraries(daemon_test ${LIBUUID_LIBRARY})
target_include_directories(daemon PRIVATE ${LIBUUID_INCLUDE_DIR})

# google test deps(require pthread)
find_package(Threads REQUIRED)
find_package(GTest CONFIG REQUIRED)
target_link_libraries(daemon_test Threads::Threads)
target_link_libraries(daemon_test GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

###############################################################################
## Compile Config #############################################################
###############################################################################
# build type specific options
if (CMAKE_BUILD_TYPE)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_options(daemon PRIVATE -o0 -g -fsanitize=address -fno-omit-frame-pointer)
        target_compile_options(daemon PRIVATE -O0 -g -fsanitize=address -fno-omit-frame-pointer)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(daemon PRIVATE -O3 -fPIC -fast-math -Bsymbolic -Werror)
    endif ()
endif ()

# general  compile options
add_compile_options(-Wall -Wextra)

# compile under cxx 17 since need std::byte support
target_compile_features(daemon PRIVATE cxx_std_17)
target_compile_features(daemon_test PRIVATE cxx_std_17)

###############################################################################
## File #######################################################################
###############################################################################
#add_subdirectory(file)

###############################################################################
## Project Utils ##############################################################
###############################################################################
add_subdirectory(utils)

###############################################################################
## Message Packet ##############################################################
###############################################################################
add_subdirectory(packet)

###############################################################################
## Commands ###################################################################
###############################################################################
add_subdirectory(interface)

###############################################################################
## Management KIT #############################################################
###############################################################################
add_subdirectory(management_api)
target_link_libraries(daemon management_api)
target_include_directories(daemon PRIVATE ${management_api_INCLUDE_DIRS})

###############################################################################
## Project Source #############################################################
###############################################################################
target_sources(daemon
        PRIVATE
        "environment.h"
        "file_database.cc" "file_database.h" "main.cc"
        "manage_interface.cc" "manage_interface.h"
        "peer_session.cc" "p2p_server_context.cc" "p2p_interface.h")
target_include_directories(daemon PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

###############################################################################
## Testing ####################################################################
###############################################################################
# scan for possibale test
gtest_discover_tests(daemon_test)
