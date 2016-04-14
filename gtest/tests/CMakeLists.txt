include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(libdwgx)

add_executable(run_libdwgx_tests
        io_check.cpp reading_geometries.cpp)

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(run_libdwgx_tests gtest gtest_main)
target_link_libraries(run_libdwgx_tests libdwgx_core)