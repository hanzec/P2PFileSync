###############################################################################
## Server Protocol Utils ######################################################
###############################################################################
add_library(server_connection)

# test target
add_executable(server_connection_test)

# response model should be expose as public header
file(GLOB RESPONSE_MODEL_HDRS ${CMAKE_CURRENT_LIST_DIR}/model/request/*.h)

# response model should be expose as public header
set_target_properties(server_connection PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(server_connection PROPERTIES PUBLIC_HEADER "${RESPONSE_MODEL_HDRS}")

###############################################################################
## Compile Options ############################################################
###############################################################################
# compile options
add_compile_options(-Wall -Wextra -Bsymbolic -fPIC)

# compile as c++ 17
target_compile_features(server_connection PRIVATE cxx_std_17)
target_compile_features(server_connection_test PRIVATE cxx_std_17)


# set by default will not export symbols
set_target_properties(server_connection PROPERTIES CXX_VISIBILITY_PRESET hidden)

###############################################################################
## Dependency #################################################################
###############################################################################
#libcurl
find_package(CURL REQUIRED)
target_link_libraries(server_connection ${CURL_LIBRARIES})
target_link_libraries(server_connection_test ${CURL_LIBRARIES})
target_include_directories(server_connection PRIVATE ${CURL_INCLUDE_DIR})

#rapidJson
find_package(RapidJSON REQUIRED)
target_include_directories(server_connection PRIVATE ${RAPIDJSON_INCLUDE_DIR})

# openssl
find_package(OpenSSL REQUIRED)
target_link_libraries(server_connection OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(server_connection_test OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(server_connection PRIVATE ${OPENSSL_INCLUDE_DIR})

# google log
find_package(glog CONFIG REQUIRED)
target_link_libraries(server_connection glog::glog)
target_link_libraries(server_connection_test glog::glog)
target_include_directories(server_connection PRIVATE ${glog_INCLUDE_DIRS})

# uuid utils
find_path(LIBUUID_INCLUDE_DIR uuid.h PATH_SUFFIXES uuid)
find_library(LIBUUID_LIBRARY libuuid.a)
target_link_libraries(server_connection ${LIBUUID_LIBRARY})
target_include_directories(server_connection PRIVATE ${LIBUUID_INCLUDE_DIR})

# google test deps(require pthread)
find_package(Threads REQUIRED)
find_package(GTest CONFIG REQUIRED)
target_link_libraries(server_connection_test Threads::Threads)
target_link_libraries(server_connection_test GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

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
file(GLOB SERVER_KIT_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cc)
file(GLOB SERVER_KIT_HDRS ${CMAKE_CURRENT_LIST_DIR}/*.hpp)

target_sources(server_connection
        PUBLIC
        "server_kit.h"
        PRIVATE
        ${REQUEST_MODEL_SRCS}
        ${UTILS_SRCS} ${UTILS_HDRS}
        ${SERVER_KIT_SRCS} ${SERVER_KIT_HDRS})

target_include_directories(server_connection PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

###############################################################################
## Testing ####################################################################
###############################################################################
# test main
target_sources(server_connection_test PRIVATE "test_main.cc")

# scan for possibale test
gtest_discover_tests(server_connection_test)
