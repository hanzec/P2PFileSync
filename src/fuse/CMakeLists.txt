###############################################################################
## User space Filesystem ######################################################
###############################################################################
add_library(fuse_mount VERSION 0.1.0)

###############################################################################
## Fuse Backend ###############################################################
###############################################################################
# if under unix will using libfuse mounting folder
if (UNIX)
    find_package(FUSE REQUIRED)
    target_link_libraries(fuse_mount ${FUSE_LIBRARIES})
    target_include_directories(fuse_mount PRIVATE ${FUSE_INCLUDE_DIR})
endif(UNIX)
    
