include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(libopencad)

add_executable(run_libopencad_tests
        io_check.cpp reading_geometries.cpp)

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(run_libopencad_tests gtest gtest_main)
target_link_libraries(run_libopencad_tests libopencad_lib)