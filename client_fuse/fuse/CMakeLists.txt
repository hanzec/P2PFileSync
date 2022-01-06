include(FetchFUSE)

###############################################################################
## User space Filesystem ######################################################
###############################################################################
add_library(fuse_client)

###############################################################################
## Dependency #################################################################
###############################################################################
# user space filesystem will implement using fuse under maxos and unix-like system
if(APPLE OR UNIX)
  find_package(libfuse REQUIRED)
  target_link_libraries(fuse_client ${libfuse_LIBRARIES})
  target_include_directories(fuse_client PRIVATE ${libfuse_INCLUDE_DIR})
endif(APPLE OR UNIX)

###############################################################################
## Library Sources ############################################################
###############################################################################
target_sources(fuse_client PRIVATE
  "fuse_op.cc"
)
