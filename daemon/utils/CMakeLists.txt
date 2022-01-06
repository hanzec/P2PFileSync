###############################################################################
## Cert Utils #################################################################
###############################################################################
add_subdirectory(cert)

###############################################################################
## data_structs ###############################################################
###############################################################################
file(GLOB P2PSYNC_UTILS_DATA_STRUCT_HDRS
        ${CMAKE_CURRENT_LIST_DIR}/data_struct/*.h)
file(GLOB P2PSYNC_UTILS_DATA_STRUCT_SRCS
        ${CMAKE_CURRENT_LIST_DIR}/data_struct/*.cc)

target_sources(daemon PRIVATE ${P2PSYNC_UTILS_DATA_STRUCT_HDRS} ${P2PSYNC_UTILS_DATA_STRUCT_SRCS})

###############################################################################
## Utils Source ###############################################################
###############################################################################
file(GLOB P2PSYNC_UTILS_HDRS
        ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB P2PSYNC_UTILS_SRCS
        ${CMAKE_CURRENT_LIST_DIR}/*.cc)

target_sources(daemon PRIVATE ${P2PSYNC_UTILS_HDRS} ${P2PSYNC_UTILS_SRCS})

###############################################################################
## Testing Source #############################################################
###############################################################################
file(GLOB TEST_SRC_FILES
        ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

target_sources(daemon_test PRIVATE
        ${TEST_SRC_FILES}
        ${P2PSYNC_UTILS_SRCS} ${P2PSYNC_UTILS_HDRS}
        ${P2PSYNC_UTILS_DATA_STRUCT_HDRS} ${P2PSYNC_UTILS_DATA_STRUCT_SRCS})