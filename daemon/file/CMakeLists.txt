###############################################################################
## Models #####################################################################
###############################################################################
file(GLOB FILE_MODELS_HDRS ${CMAKE_CURRENT_LIST_DIR}/models/*.h)
file(GLOB FILE_MODELS_SRCS ${CMAKE_CURRENT_LIST_DIR}/models/*.cc)
target_sources(daemon PRIVATE ${FILE_MODELS_SRCS} ${FILE_MODELS_HDRS})

###############################################################################
## Sources ####################################################################
###############################################################################
file(GLOB DAEMON_FILE_HDRS ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB DAEMON_FILE_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(daemon
        PRIVATE
        ${DAEMON_FILE_SRCS} ${DAEMON_FILE_HDRS})