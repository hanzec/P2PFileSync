###############################################################################
## Common Targets #############################################################
###############################################################################
# header only target
add_library(common INTERFACE)

# test target
add_executable(common_test)
###############################################################################
## Shared Dependency ##########################################################
###############################################################################
# google test deps(require pthread)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(common_test Threads::Threads)
target_link_libraries(common_test ${GTEST_LIBRARY})
target_include_directories(common_test PRIVATE ${GTEST_INCLUDE_DIRS})

###############################################################################
## Compile Config #############################################################
###############################################################################
# compile under cxx 17 since need std::byte support
target_compile_features(common_test PRIVATE cxx_std_17)

###############################################################################
## Common Utils Source ########################################################
###############################################################################
target_include_directories(common INTERFACE ${CMAKE_CURRENT_LIST_DIR})

###############################################################################
## Common Utils Test ##########################################################
###############################################################################
file(GLOB TEST_SRC_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)
target_sources(common_test PRIVATE ${TEST_SRC_FILES} "test_main.cc")