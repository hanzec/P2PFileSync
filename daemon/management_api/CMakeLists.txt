###############################################################################
## Management KIT #############################################################
###############################################################################
add_library(management_api SHARED)

# test target
add_executable(management_api_test)

###############################################################################
## Public Header ##############################################################
###############################################################################
# collect all headers
file(GLOB_RECURSE management_api_PUBLIC_HEADER_LIST "include/*.h")

# include all headers
target_include_directories(management_api PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

# expose current include directory
set(management_api_INCLUDE_DIRS ${management_api_PUBLIC_HEADER_LIST} PARENT_SCOPE)

###############################################################################
## Library Configuration ######################################################
###############################################################################
# include current directory as private heads
target_include_directories(management_api PRIVATE .)

# set management api version
set(management_api_VERSION_MAJOR 0)
set(management_api_VERSION_MINOR 0)
set(management_api_VERSION_PATCH 1)
set(management_api_VERSION_STRING
        ${management_api_VERSION_MAJOR}.${management_api_VERSION_MINOR}.${management_api_VERSION_PATCH})

set_target_properties(management_api PROPERTIES
        VERSION ${management_api_VERSION_STRING}
        SOVERSION ${management_api_VERSION_MAJOR})

# target configuration
set_target_properties(management_api PROPERTIES MACOSX_RPATH ON)
set_target_properties(management_api PROPERTIES POSITION_INDEPENDENT_CODE 1) # generate fpic
set_target_properties(management_api PROPERTIES VISIBILITY_INLINES_HIDDEN 1) # generate fpic
set_target_properties(management_api PROPERTIES C_VISIBILITY_PRESET hidden) # not export symbols
set_target_properties(management_api PROPERTIES CXX_VISIBILITY_PRESET hidden) # not export symbols
set_target_properties(management_api PROPERTIES OUTPUT_NAME "management_kit") # export lib name
set_target_properties(management_api PROPERTIES PUBLIC_HEADER "${management_api_PUBLIC_HEADER_LIST}") # export header name

###############################################################################
## Management API Versions ####################################################
###############################################################################
# generate build time
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")

# collect head commit hash
set(GIT_HASH "")
get_git_hash(GIT_HASH)

# rewrite the version string
set(management_api_FULL_VERSION
        ${management_api_VERSION_STRING}-${GIT_HASH}\(${BUILD_TIMESTAMP}\))

# generate the configuration file
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.h)
target_include_directories(management_api PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

###############################################################################
## Compile Options ############################################################
###############################################################################
# build type specific options
if (CMAKE_BUILD_TYPE)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_options(management_api PRIVATE -o0 -g -fsanitize=address -fno-omit-frame-pointer)
        target_compile_options(management_api PRIVATE -O0 -g -fsanitize=address -fno-omit-frame-pointer)
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(management_api PRIVATE -O3 -ffast-math -Bsymbolic -Werror)
    endif ()
endif ()

# compile options
target_compile_options(management_api PRIVATE -Wall -Wextra)

# hide static link symbols
if(MACOSX)
    target_link_options(management_api PRIVATE -hidden-lssl)
else()
    target_link_options(management_api PRIVATE -Wl,--exclude-libs,ALL)
endif()

# compile as c++ 17
target_compile_features(management_api PRIVATE cxx_std_17)
target_compile_features(management_api_test PRIVATE cxx_std_17)

###############################################################################
## Dependency #################################################################
###############################################################################
#libcurl
find_package(CURL CONFIG REQUIRED)
target_link_libraries(management_api ${CURL_LIBRARIES})
target_link_libraries(management_api_test ${CURL_LIBRARIES})
target_include_directories(management_api PRIVATE ${CURL_INCLUDE_DIRS})

#rapidJson
find_package(RapidJSON CONFIG REQUIRED)
target_include_directories(management_api PRIVATE ${RAPIDJSON_INCLUDE_DIRS})

# openssl
find_package(OpenSSL REQUIRED)
target_link_libraries(management_api OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(management_api_test OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(management_api PRIVATE ${OPENSSL_INCLUDE_DIR})

# google log
find_package(glog CONFIG REQUIRED)
target_link_libraries(management_api glog::glog)
target_link_libraries(management_api_test glog::glog)
target_include_directories(management_api PRIVATE ${glog_INCLUDE_DIRS})

# uuid utils
find_path(LIBUUID_INCLUDE_DIR uuid.h PATH_SUFFIXES uuid)
find_library(LIBUUID_LIBRARY libuuid.a)
target_link_libraries(management_api ${LIBUUID_LIBRARY})
target_include_directories(management_api PRIVATE ${LIBUUID_INCLUDE_DIR})

# google test deps(require pthread)
find_package(Threads REQUIRED)
find_package(GTest CONFIG REQUIRED)
target_link_libraries(management_api_test Threads::Threads)
target_link_libraries(management_api_test GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

###############################################################################
## Data Models ################################################################
###############################################################################
add_subdirectory(model)

###############################################################################
## UTILS ######################################################################
###############################################################################
add_subdirectory(utils)

###############################################################################
## Library Sources ############################################################
###############################################################################
target_sources(management_api
        PRIVATE
        "user_context.cc"
        "version.h" "user_context.cc"
        "management_api_impl.h" "server_endpoint.h"
        "device_context.cc" "management_context.cc")

###############################################################################
## Testing ####################################################################
###############################################################################
# test main
target_sources(management_api_test PRIVATE "test_main.cc")

# scan for possibale test
gtest_discover_tests(management_api_test)

###############################################################################
## Installing #################################################################
###############################################################################
install(TARGETS management_api
        EXPORT management_api-targets
        PUBLIC_HEADER DESTINATION include
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(EXPORT management_api-targets
        NAMESPACE ManagementAPI::
        FILE management_api-config.cmake
        DESTINATION lib/cmake/management_api)
