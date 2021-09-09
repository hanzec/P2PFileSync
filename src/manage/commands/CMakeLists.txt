###############################################################################
## Project Source #############################################################
###############################################################################
file(GLOB SUPPORT_COMMAND_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cc)

target_sources(p2p_file_sync PRIVATE ${SUPPORT_COMMAND_SOURCES})

target_include_directories(p2p_file_sync PRIVATE "command.h")

###############################################################################
## Testing Source #############################################################
###############################################################################
file(GLOB TEST_SRC_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

target_sources(test_target PRIVATE 
    ${TEST_SRC_FILES}
)