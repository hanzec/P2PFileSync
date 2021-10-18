###############################################################################
## Cert Utils #################################################################
###############################################################################
file(GLOB CERT_UTILS_SRCS  ${CMAKE_CURRENT_LIST_DIR}/cert_utils_*.cc)
    
target_sources(p2p_file_sync PRIVATE ${CERT_UTILS_SRCS})
target_include_directories(p2p_file_sync PRIVATE "cert_utils.h")