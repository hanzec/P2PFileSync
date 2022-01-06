###############################################################################
## Utils Source ###############################################################
###############################################################################
file(GLOB MANAGEMENT_API_UTILS_HDRS ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB MANAGEMENT_API_UTILS_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cc)

target_sources(management_api PRIVATE ${MANAGEMENT_API_UTILS_SRCS} ${MANAGEMENT_API_UTILS_HDRS})

###############################################################################
## Testing Source #############################################################
###############################################################################
file(GLOB MANAGEMENT_API_UTILS_TEST_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

target_sources(management_api_test
        PRIVATE
        ${MANAGEMENT_API_UTILS_TEST_SRC_FILES}
        ${MANAGEMENT_API_UTILS_SRCS} ${MANAGEMENT_API_UTILS_HDRS})