cmake_minimum_required(VERSION 3.16.0)

###############################################################################
## Prepaire Third-party Dependency ############################################
###############################################################################
# downloading all submodules
execute_process(COMMAND git submodule update --init --recursive 
                RESULT_VARIABLE GIT_CLONE_STATUS WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
if(GIT_CLONE_STATUS AND NOT GIT_CLONE_STATUS EQUAL 0)
    message(FATAL_ERROR "git clone submodule failed, can't get Third-party Dependency !")
endif()

# generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add custom camake scrips loaction
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

###############################################################################
## Setup vcpkg ################################################################
###############################################################################
# run vpkg bootstarup
if((EXISTS "${CMAKE_SOURCE_DIR}/third-party/vcpkg") OR (EXISTS "${CMAKE_SOURCE_DIR}/third-party/vcpkg.exe"))
    message(VERBOSE "Found builded vcpkg at /third-party/vcpkg")
else()
    message(STATUS " Not Found builded vcpkg, trying to run bootstrap-vcpkg scripts")
    if(WIN32)
        execute_process(COMMAND "${CMAKE_SOURCE_DIR}/third-party/bootstrap-vcpkg.bat"
                        RESULT_VARIABLE VCPKG_STATUS WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
        execute_process(COMMAND "${CMAKE_SOURCE_DIR}/third-party/vcpkg.exe" integrate install
                        RESULT_VARIABLE VCPKG_STATUS WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    else()
        execute_process(COMMAND bash "${CMAKE_SOURCE_DIR}/third-party/bootstrap-vcpkg.sh"
                        RESULT_VARIABLE VCPKG_STATUS WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    endif()
    if(VCPKG_STATUS AND NOT VCPKG_STATUS EQUAL 0)
        message(FATAL_ERROR "vcpkg bootstrap failed, can't build vcpkg !")
    endif()
endif()

# set vcpkg to cmake toochain file
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(EXISTS "${CMAKE_SOURCE_DIR}/third-party/vcpkg/scripts/buildsystems/vcpkg.cmake")
    message(VERBOSE "found cmake scripts at /third-party/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/third-party/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR " Not Found vcpkg cmake scripts at /third-party/scripts/buildsystems/vcpkg.cmake !")
  endif()
else()
   message(VERBOSE "CMAKE_TOOLCHAIN_FILE is already define as ${CMAKE_TOOLCHAIN_FILE}")
endif()

###############################################################################
## Project Config #############################################################
###############################################################################
project(p2p_file_sync VERSION 0.1.0)

# initial test
enable_testing()
include(GoogleTest)

# lib fuse extension
add_library(fuse_mount)

# exec target
add_executable(p2p_file_sync)

# test target
add_executable(test_target)

###############################################################################
## Dependency #################################################################
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
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
target_link_libraries(p2p_file_sync OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(p2p_file_sync PRIVATE ${OPENSSL_INCLUDE_DIR})

# yaml-cpp
find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(p2p_file_sync ${YAML_CPP_LIBRARIES})
target_include_directories(p2p_file_sync PRIVATE ${YAML_CPP_INCLUDE_DIR})

# google test deps
find_package(GTest REQUIRED)
target_link_libraries(test_target ${GTEST_LIBRARY})
target_include_directories(test_target PRIVATE ${GTEST_INCLUDE_DIRS})

###############################################################################
## Compile Config #############################################################
###############################################################################
# compile under cxx 17 since need std::byte support
target_compile_features(test_target PRIVATE cxx_std_17)
target_compile_features(p2p_file_sync PRIVATE cxx_std_17)

# compile defs for multi-platform compiling
if(WIN32)
  add_compile_definitions(UNDER_WINDOWS)
elseif(UNIX)
  add_compile_definitions(UNDER_UNIX)
endif()

###############################################################################
## Source #####################################################################
###############################################################################
add_subdirectory(src)
target_include_directories(test_target PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_include_directories(p2p_file_sync PRIVATE "${CMAKE_SOURCE_DIR}/src")

###############################################################################
## Testing ####################################################################
###############################################################################
# scan for possibale test
gtest_discover_tests(test_target)

###############################################################################
## Packing ####################################################################
###############################################################################
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
