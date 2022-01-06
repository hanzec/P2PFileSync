include(FetchContent)
FetchContent_Declare(
        libfuse-git
        GIT_REPOSITORY https://github.com/libfuse/libfuse.git
        GIT_TAG fuse-3.10.5
        GIT_SHALLOW    TRUE
)

FetchContent_GetProperties(libfuse-git)
if (NOT libfuse-git_POPULATED)
    FetchContent_Populate(libfuse-git)
endif ()

add_custom_command(
        OUTPUT ${libfuse-git_SOURCE_DIR}/build/lib/libfuse3.so
        COMMAND third-party/libfuse/build/lib/libfuse3.so && meson ..&& ninja
        WORKING_DIRECTORY ${libfuse-git_SOURCE_DIR}
        COMMENT "build libfuse"
        VERBATIM
)

add_custom_target(libfuse-git DEPENDS ${libfuse-git_SOURCE_DIR}/build/lib/libfuse3.so)

add_library(libfuse SHARED IMPORTED)
set_property(TARGET libfuse PROPERTY IMPORTED_LOCATION ${libfuse-git_SOURCE_DIR}/build/lib/libfuse3.so)
set_target_properties(
        libfuse PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${libfuse-git_SOURCE_DIR}/include"
)
add_dependencies(libfuse libfuse-git)