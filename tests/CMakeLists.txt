################################################################################
#  Project: libopencad
#  Purpose: OpenSource CAD formats support library
#  Author: Alexandr Borzykh, mush3d at gmail.com
#  Author: Dmitry Baryshnikov, bishop.dev@gmail.com
#  Language: C++
################################################################################
#  The MIT License (MIT)
#
#  Copyright (c) 2016 Alexandr Borzykh
#  Copyright (c) 2016 NextGIS, <info@nextgis.com>
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.
################################################################################

option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    include(CTest)
    
    set(WITH_GTest ON CACHE BOOL "Google test on")
    set(WITH_GTest_EXTERNAL ON CACHE BOOL "Google test external on")
    find_anyproject(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src
                        ${CMAKE_SOURCE_DIR}/lib)

    
    find_package(Threads)
    set(TARGET_LINK_LIB ${TARGET_LINK_LIB} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} ${TARGET_LINK})

    find_library(M_LIB m)
    set(TARGET_LINK_LIB ${TARGET_LINK_LIB} ${M_LIB})

    file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY data/r2000 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)

    add_executable(io_test
                   io_check.cpp)
    target_link_extlibraries(io_test)
    add_test( io_test io_test )

    add_executable(geometry_test
                   reading_geometries.cpp)
    target_link_extlibraries(geometry_test)
    add_test( geometry_test geometry_test )

endif()
