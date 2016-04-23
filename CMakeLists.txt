cmake_minimum_required(VERSION 3.2.2)
project(lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror")

include_directories(lib)

add_subdirectory(lib)
add_subdirectory(tests)