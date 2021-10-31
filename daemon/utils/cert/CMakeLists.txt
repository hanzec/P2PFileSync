###############################################################################
## Cert Utils #################################################################
###############################################################################
file(GLOB CERT_UTILS_SRCS  ${CMAKE_CURRENT_LIST_DIR}/cert_utils_*.cc)
    
target_sources(daemon PRIVATE ${CERT_UTILS_SRCS})
target_include_directories(daemon PRIVATE "cert_utils.h")