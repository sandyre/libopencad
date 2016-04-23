cmake_minimum_required(VERSION 3.2.2)
project(libopencad)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp src/dwg/dwg_io.cpp src/dwg/dwg_r2000.cpp src/cadfile.cpp src/libopencad_api.cpp)
add_executable(libopencad_exe ${SOURCE_FILES})
add_library(libopencad_lib ${SOURCE_FILES})

#target_link_libraries(libopencad_exe libopencad_lib)