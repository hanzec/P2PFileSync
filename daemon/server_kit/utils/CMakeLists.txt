###############################################################################
## Utils Source ###############################################################
###############################################################################
file(GLOB SERVER_KIT_UTILS_HDRS ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB SERVER_KIT_UTILS_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cc)

target_sources(server_connection PRIVATE ${SERVER_KIT_UTILS_SRCS} ${SERVER_KIT_UTILS_HDRS})

###############################################################################
## Testing Source #############################################################
###############################################################################
file(GLOB SERVER_KIT_UTILS_TEST_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

target_sources(server_connection_test
        PRIVATE
        ${SERVER_KIT_UTILS_TEST_SRC_FILES}
        ${SERVER_KIT_UTILS_SRCS} ${SERVER_KIT_UTILS_HDRS})