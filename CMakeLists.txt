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

###############################################################################
## Compile Config #############################################################
###############################################################################
# compile defs for multi-platform compiling
if(WIN32)
  add_compile_definitions(UNDER_WINDOWS)
elseif(UNIX)
  add_compile_definitions(UNDER_UNIX)
endif()

###############################################################################
## Daemon Source ##############################################################
###############################################################################
add_subdirectory(daemon)

###############################################################################
## Packing ####################################################################
###############################################################################
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
