include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

enable_testing()

add_executable(run_libopencad_tests
        io_check.cpp reading_geometries.cpp)

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY data/r2000 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)

target_link_libraries(run_libopencad_tests gtest gtest_main)
target_link_libraries(run_libopencad_tests ${TARGET_LINK})

add_test( run_libopencad_tests run_libopencad_tests)