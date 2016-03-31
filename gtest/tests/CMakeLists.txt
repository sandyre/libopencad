include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(libdwgx)

add_executable(run_libdwgx_tests
        io_check.cpp)

target_link_libraries(run_libdwgx_tests gtest gtest_main)