###############################################################################
## Data Model Base ############################################################
###############################################################################
target_sources(server_connection PRIVATE "IJsonModel.h")

###############################################################################
## Data Model Source ##########################################################
###############################################################################
file(GLOB DATA_MODEL_HDRS ${CMAKE_CURRENT_LIST_DIR}/data/*.h)
file(GLOB DATA_MODEL_SRCS ${CMAKE_CURRENT_LIST_DIR}/data/*.cc)

target_sources(server_connection
        PRIVATE
        ${DATA_MODEL_HDRS} ${DATA_MODEL_SRCS})

###############################################################################
## Request Model Source #######################################################
###############################################################################
file(GLOB REQUEST_MODEL_HDRS ${CMAKE_CURRENT_LIST_DIR}/request/*.h)
file(GLOB REQUEST_MODEL_SRCS ${CMAKE_CURRENT_LIST_DIR}/request/*.cc)

target_sources(server_connection
        PRIVATE
        ${REQUEST_MODEL_HDRS} ${REQUEST_MODEL_SRCS})

###############################################################################
## Response Model Source ######################################################
###############################################################################
file(GLOB RESPONSE_MODEL_HDRS ${CMAKE_CURRENT_LIST_DIR}/response/*.h)
file(GLOB RESPONSE_MODEL_SRCS ${CMAKE_CURRENT_LIST_DIR}/response/*.cc)
file(GLOB USER_RESPONSE_MODEL_HDRS ${CMAKE_CURRENT_LIST_DIR}/response/user/*.h)
file(GLOB USER_RESPONSE_MODEL_SRCS ${CMAKE_CURRENT_LIST_DIR}/response/user/*.cc)
file(GLOB CLIENT_RESPONSE_MODEL_HDRS ${CMAKE_CURRENT_LIST_DIR}/response/client/*.h)
file(GLOB CLIENT_RESPONSE_MODEL_SRCS ${CMAKE_CURRENT_LIST_DIR}/response/client/*.cc)

target_sources(server_connection
        PRIVATE
        "IJsonModel.h"
        ${RESPONSE_MODEL_HDRS} ${RESPONSE_MODEL_SRCS}
        ${USER_RESPONSE_MODEL_HDRS} ${USER_RESPONSE_MODEL_SRCS}
        ${CLIENT_RESPONSE_MODEL_HDRS} ${CLIENT_RESPONSE_MODEL_SRCS})

###############################################################################
## Testing Source #############################################################
###############################################################################
