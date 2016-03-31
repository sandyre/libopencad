cmake_minimum_required(VERSION 3.3)
project(libdwgx)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp libdwgx.cpp dwg_r2000.cpp libdwgx_io.cpp)