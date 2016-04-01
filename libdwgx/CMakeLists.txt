cmake_minimum_required(VERSION 3.2.2)
project(libdwgx)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp libdwgx.cpp dwg_r2000.cpp libdwgx_io.cpp)
add_executable(libdwgx_exe ${SOURCE_FILES})
add_library(libdwgx_core ${SOURCE_FILES})

target_link_libraries(libdwgx_exe libdwgx_core)