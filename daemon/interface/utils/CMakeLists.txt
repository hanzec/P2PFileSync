###############################################################################
## Utils Source ###############################################################
###############################################################################
file(GLOB INTERFACE_UTILS_HDRS ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB INTERFACE_UTILS_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cc)

target_sources(daemon PRIVATE ${INTERFACE_UTILS_HDRS} ${INTERFACE_UTILS_SRCS})

###############################################################################
## Testing Source #############################################################
###############################################################################
file(GLOB INTERFACE_UTILS_TEST_SRCS ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

target_sources(daemon_test
        PRIVATE
        ${INTERFACE_UTILS_TEST_SRCS}
        ${INTERFACE_UTILS_HDRS} ${INTERFACE_UTILS_SRCS})