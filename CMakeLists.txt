cmake_minimum_required(VERSION 3.16.0)

###############################################################################
## CMake Settings #############################################################
###############################################################################

###############################################################################
## Compile Config #############################################################
###############################################################################
# detect is MACOS or not
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

# compile defs for multi-platform compiling
if(WIN32)
    add_compile_definitions(WIN32)
    set(VCPKG_TARGET_TRIPLET x86-windows-static)
elseif(UNIX AND NOT MACOSX)
    add_compile_definitions(UNIX)
    set(VCPKG_TARGET_TRIPLET x64-linux)
elseif(MACOSX)
    add_compile_definitions(MACOSX)
endif()

###############################################################################
## Prepaire Third-party Dependency ############################################
###############################################################################
# downloading all submodules
if( NOT EXISTS "${CMAKE_SOURCE_DIR}/third-party/vcpkg" )
    execute_process(COMMAND git submodule update --init --recursive
            RESULT_VARIABLE GIT_CLONE_STATUS WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    if(GIT_CLONE_STATUS AND NOT GIT_CLONE_STATUS EQUAL 0)
        message(FATAL_ERROR "git clone submodule failed, can't get Third-party Dependency !")
    endif()
endif()

# generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add custom camake scrips loaction
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

###############################################################################
## Setup vcpkg ################################################################
###############################################################################
# run vpkg bootstarup
if((EXISTS "${CMAKE_SOURCE_DIR}/third-party/vcpkg/vcpkg") OR (EXISTS "${CMAKE_SOURCE_DIR}/third-party/vcpkg/vcpkg.exe"))
    message(VERBOSE "Found builded vcpkg at ${CMAKE_SOURCE_DIR}/third-party/vcpkg")
else()
    message(STATUS "Not Found builded vcpkg, trying to run bootstrap-vcpkg scripts")
    if(WIN32)
        execute_process(COMMAND "${CMAKE_SOURCE_DIR}/third-party/vcpkg/bootstrap-vcpkg.bat"
                        RESULT_VARIABLE VCPKG_STATUS WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
        execute_process(COMMAND "${CMAKE_SOURCE_DIR}/third-party/vcpkg/vcpkg.exe" integrate install
                        RESULT_VARIABLE VCPKG_STATUS WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    else()
        execute_process(COMMAND bash "${CMAKE_SOURCE_DIR}/third-party/vcpkg/bootstrap-vcpkg.sh"
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

include_directories(daemon/management_api/include/P2PFileSync)

# project lib export folder
SET(LIBRARY_OUTPUT_PATH      ${PROJECT_BINARY_DIR}/lib)

###############################################################################
## Testing Source #############################################################
###############################################################################
# initial test
enable_testing()
include(GoogleTest)


###############################################################################
## File Sync Daemon ###########################################################
###############################################################################
add_subdirectory(daemon)

###############################################################################
## Client(Fuse) Source ########################################################
###############################################################################
#add_subdirectory(client_fuse) // TODO not ready right now

###############################################################################
## Packing ####################################################################
###############################################################################
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
