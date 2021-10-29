###############################################################################
## Server Protocol Utils ######################################################
###############################################################################
add_library(server_connection)

set_target_properties(server_connection PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "Serverkit.h")

set_source_files_properties("Serverkit.h" LANGUAGE C)
target_compile_features(server_connection PRIVATE cxx_std_17)

###############################################################################
## Compile Options ############################################################
###############################################################################
# compile options
add_compile_options(-Wall -Wextra -Bsymbolic -fPIC)

# set by default will not export symbols
set_target_properties(server_connection PROPERTIES CXX_VISIBILITY_PRESET hidden)

###############################################################################
## Dependency #################################################################
###############################################################################
#libcurl
find_package(CURL REQUIRED)
target_link_libraries(server_connection ${CURL_LIBRARIES})
target_include_directories(server_connection PRIVATE ${CURL_INCLUDE_DIR})

#rapidJson
find_package(RapidJSON REQUIRED)
target_include_directories(server_connection PRIVATE ${RAPIDJSON_INCLUDE_DIR})

# openssl
find_package(OpenSSL REQUIRED)
target_link_libraries(server_connection OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(server_connection PRIVATE ${OPENSSL_INCLUDE_DIR})

# google log
find_package(glog CONFIG REQUIRED)
target_link_libraries(server_connection glog::glog)
target_include_directories(server_connection PRIVATE ${glog_INCLUDE_DIRS})

###############################################################################
## UTILS ######################################################################
###############################################################################
file(GLOB UTILS_SRCS ${CMAKE_CURRENT_LIST_DIR}/utils/*.cc)
file(GLOB UTILS_HDRS ${CMAKE_CURRENT_LIST_DIR}/utils/*.hpp)
    
###############################################################################
## Data Models ################################################################
###############################################################################
add_subdirectory(model)

###############################################################################
## Library Sources ############################################################
###############################################################################
target_sources(server_connection
  PUBLIC
  "server_kit.h"
  PRIVATE
  "export.h"
  ${UTILS_SRCS} ${UTILS_HDRS}
  ${REQUEST_MODEL_SRCS}
  "server_kit.cc" "server_kit.h")

target_include_directories(server_connection PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

