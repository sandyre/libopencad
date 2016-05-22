################################################################################
# Project:  external projects
# Purpose:  CMake build scripts
# Author:   Dmitry Baryshnikov, polimax@mail.ru
################################################################################
# Copyright (C) 2015-2016, NextGIS <info@nextgis.com>
# Copyright (C) 2015 Dmitry Baryshnikov
#
# This script is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This script is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this script.  If not, see <http://www.gnu.org/licenses/>.
################################################################################

set(TARGET_LINK_LIB) # ${TARGET_LINK_LIB} ""
set(DEPENDENCY_LIB) # ${DEPENDENCY_LIB} ""
set(WITHOPT ${WITHOPT} "")
set(EXPORTS_PATHS)
set(LINK_SEARCH_PATHS)
      
include (CMakeParseArguments)  

function(find_anyproject name)
    set(options OPTIONAL REQUIRED QUIET EXACT MODULE)
    set(oneValueArgs DEFAULT VERSION SHARED)
    set(multiValueArgs CMAKE_ARGS COMPONENTS)
    cmake_parse_arguments(find_anyproject "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )  
    
    if (find_anyproject_REQUIRED OR find_anyproject_DEFAULT)
        set(_WITH_OPTION_ON TRUE)
    else()  
        set(_WITH_OPTION_ON FALSE)
    endif()
    
    if(DEFINED WITH_${name})
        set(WITHOPT "${WITHOPT}option(WITH_${name} \"Set ON to use ${name}\" ${WITH_${name}})\n")
        if(DEFINED WITH_${name}_EXTERNAL)
            set(WITHOPT "${WITHOPT}option(WITH_${name}_EXTERNAL \"Set ON to use external ${name}\" ${WITH_${name}_EXTERNAL})\n")
        else()
            set(WITHOPT "${WITHOPT}option(WITH_${name}_EXTERNAL \"Set ON to use external ${name}\" OFF)\n")
        endif()
    else()
        set(WITHOPT "${WITHOPT}option(WITH_${name} \"Set ON to use ${name}\" ${_WITH_OPTION_ON})\n")
        set(WITHOPT "${WITHOPT}option(WITH_${name}_EXTERNAL \"Set ON to use external ${name}\" OFF)\n")
        option(WITH_${name} "Set ON to use ${name}" ${_WITH_OPTION_ON})
    endif()

    string(TOUPPER ${name}_FOUND IS_FOUND)
    string(TOUPPER ${name}_VERSION_STRING VERSION_STRING)
    if(NOT DEFINED ${IS_FOUND}) #if the package was found anywhere
        set(${IS_FOUND} FALSE)
    endif()
    string(TOUPPER ${name} UPPER_NAME)

    write_ext_options(find_anyproject_SHARED)
    
    if(WITH_${name})
        option(WITH_${name}_EXTERNAL "Set ON to use external ${name}" OFF)
        if(WITH_${name}_EXTERNAL)
            include(FindExtProject)
            find_extproject(${name} ${ARGN})
        else()
            # transfer some input options to find_package arguments
            if(find_anyproject_VERSION)
                set(FIND_PROJECT_ARG ${find_anyproject_VERSION})
            endif()
            if(find_anyproject_EXACT)
                set(FIND_PROJECT_ARG ${FIND_PROJECT_ARG} EXACT)
            endif()
            if(find_anyproject_QUIET)
                set(FIND_PROJECT_ARG ${FIND_PROJECT_ARG} QUIET)
            endif()
            if(find_anyproject_MODULE)
                set(FIND_PROJECT_ARG ${FIND_PROJECT_ARG} MODULE)
            endif()
            if(find_anyproject_REQUIRED)
                set(FIND_PROJECT_ARG ${FIND_PROJECT_ARG} REQUIRED)
            endif()
            if(find_anyproject_COMPONENTS)
                set(FIND_PROJECT_ARG ${FIND_PROJECT_ARG} COMPONENTS ${find_anyproject_COMPONENTS})
            endif()
            
            find_package(${name} ${FIND_PROJECT_ARG})
        endif() 
        
        #message(STATUS "VERSION_STRING ${VERSION_STRING} ${${VERSION_STRING}}")
        if(${IS_FOUND}) 
            set(${IS_FOUND} TRUE CACHE INTERNAL "use ${name}")  
            set(${VERSION_STRING} ${${VERSION_STRING}} CACHE INTERNAL "version ${name}") 
            mark_as_advanced(${IS_FOUND})
        elseif(find_anyproject_REQUIRED)
            message(FATAL_ERROR "${name} is required in ${PROJECT_NAME}!")
        else()
            message(WARNING "${name} not found and will be disabled in ${PROJECT_NAME}!")
        endif()
    endif()
    
    if(NOT WITH_${name}_EXTERNAL AND ${IS_FOUND})
        if(${UPPER_NAME}_INCLUDE_DIRS)       
            include_directories(${${UPPER_NAME}_INCLUDE_DIRS})
        elseif(${UPPER_NAME}_INCLUDE_DIR)
            include_directories(${${UPPER_NAME}_INCLUDE_DIR})
        elseif(${name}_INCLUDE_DIRS)       
            include_directories(${${name}_INCLUDE_DIRS})
        elseif(${name}_INCLUDE_DIR)
            include_directories(${${name}_INCLUDE_DIR})    
        endif()  
        if(${UPPER_NAME}_LIBRARIES)
            set(TARGET_LINK_LIB ${TARGET_LINK_LIB} ${${UPPER_NAME}_LIBRARIES} PARENT_SCOPE)
        elseif(${UPPER_NAME}_LIBRARY)
            set(TARGET_LINK_LIB ${TARGET_LINK_LIB} ${${UPPER_NAME}_LIBRARY} PARENT_SCOPE)
        elseif(${name}_LIBRARIES)
            set(TARGET_LINK_LIB ${TARGET_LINK_LIB} ${${name}_LIBRARIES} PARENT_SCOPE)
        elseif(${name}_LIBRARY)
            set(TARGET_LINK_LIB ${TARGET_LINK_LIB} ${${name}_LIBRARY} PARENT_SCOPE)
        endif() 
    else()
        set(TARGET_LINK_LIB ${TARGET_LINK_LIB} PARENT_SCOPE)   
        set(DEPENDENCY_LIB ${DEPENDENCY_LIB} PARENT_SCOPE)    
    endif()
    set(WITHOPT ${WITHOPT} PARENT_SCOPE)
    set(EXPORTS_PATHS ${EXPORTS_PATHS} PARENT_SCOPE)
    set(LINK_SEARCH_PATHS ${LINK_SEARCH_PATHS} PARENT_SCOPE) 

    write_ext_options(find_anyproject_SHARED)
endfunction()

function(target_link_extlibraries name)
    if(DEPENDENCY_LIB)
        add_dependencies(${name} ${DEPENDENCY_LIB})  
    endif()   
    if(LINK_SEARCH_PATHS)
        link_directories(${LINK_SEARCH_PATHS})
    endif() 
    if(TARGET_LINK_LIB)
        #list(REMOVE_DUPLICATES TARGET_LINK_LIB) debug;...;optimised;... etc. if filter out
        target_link_libraries(${name} ${TARGET_LINK_LIB})
    endif()
    
endfunction()

macro(write_ext_options IS_SHARED)
    set(OUTPUT_STR ${WITHOPT})
    if(NOT ${IS_SHARED})
        if(EXPORTS_PATHS)
            foreach(EXPORT_PATH ${EXPORTS_PATHS})
                string(CONCAT EXPORTS_PATHS_STR ${EXPORTS_PATHS_STR} " \"${EXPORT_PATH}\"")
            endforeach()
            set(OUTPUT_STR "${OUTPUT_STR}set(INCLUDE_EXPORTS_PATHS \${INCLUDE_EXPORTS_PATHS} ${EXPORTS_PATHS_STR})\n")
        endif()
    endif()
    if(LINK_SEARCH_PATHS)
        foreach(LINK_SEARCH_PATH ${LINK_SEARCH_PATHS})
            string(CONCAT LINK_SEARCH_PATHS_STR ${LINK_SEARCH_PATHS_STR} " \"${LINK_SEARCH_PATH}\"")
        endforeach()
        set(OUTPUT_STR "${OUTPUT_STR}set(INCLUDE_LINK_SEARCH_PATHS \${INCLUDE_LINK_SEARCH_PATHS} ${LINK_SEARCH_PATHS_STR})\n")
    endif()
    file(WRITE ${CMAKE_BINARY_DIR}/ext_options.cmake ${OUTPUT_STR})
endmacro()
