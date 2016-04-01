cmake_minimum_required(VERSION 3.2.2)
project(libdwgx)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES libdwgx/main.cpp libdwgx/libdwgx.cpp libdwgx/dwg_r2000.cpp libdwgx/libdwgx_io.cpp)
add_executable(libdwgx_run ${SOURCE_FILES})

include_directories(libdwgx)

add_subdirectory(libdwgx)
add_subdirectory(gtest)

target_link_libraries(libdwgx_run libdwgx_core)