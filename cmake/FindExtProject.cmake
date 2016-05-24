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

function(get_imported_targets file_to_search targets)
    file(STRINGS ${file_to_search} targets_str
         REGEX "^foreach+([()_a-zA-Z0-9 ]+)")    
    string(SUBSTRING ${targets_str} 24 -1 targets_local)
    string(LENGTH ${targets_local} STR_LEN)
    math(EXPR LAST_INDEX "${STR_LEN} - 1")
    string(SUBSTRING ${targets_local} 0 ${LAST_INDEX} targets_local)  
    string(REPLACE " " ";" targets_local ${targets_local})
    set(${targets} ${targets_local} PARENT_SCOPE)
endfunction()

function(get_target_name target_lib target_name)
    string(LENGTH ${target_lib} STR_LEN)
    if(STR_LEN LESS 21)
        return()
    endif()    
    string(SUBSTRING ${target_lib} 21 -1 targets_local)
    string(LENGTH ${targets_local} STR_LEN)
    math(EXPR LAST_INDEX "${STR_LEN} - 1")
    string(SUBSTRING ${targets_local} 0 ${LAST_INDEX} targets_local)  
    set(${target_name} ${targets_local} PARENT_SCOPE)
endfunction()

function(check_updates file_path update_period check)
    file(TIMESTAMP ${file_path} LAST_PULL "%y%j%H%M" UTC)
    if(NOT LAST_PULL)
        set(LAST_PULL 0)
    endif()
    string(TIMESTAMP CURRENT_TIME "%y%j%H%M" UTC)
    math(EXPR DIFF_TIME "${CURRENT_TIME} - ${LAST_PULL}")
    #message(STATUS "period ${update_period} diff ${DIFF_TIME} current ${CURRENT_TIME} last ${LAST_PULL}")
    if(DIFF_TIME GREATER ${update_period})
        set(${check} TRUE PARENT_SCOPE)
    else()
        set(${check} FALSE PARENT_SCOPE)
    endif()
endfunction()

function(color_message text)

    string(ASCII 27 Esc)
    set(BoldGreen   "${Esc}[1;32m")
    set(ColourReset "${Esc}[m")
        
    message(STATUS "${BoldGreen}${text}${ColourReset}")
    
endfunction()

function(include_exports_path include_path)
    #add to list imported 
    list(FIND EXPORTS_PATHS ${include_path} PATH_INDEX)
    if(PATH_INDEX EQUAL -1)
        list(APPEND EXPORTS_PATHS "${include_path}")
        set(EXPORTS_PATHS "${EXPORTS_PATHS}" PARENT_SCOPE)
        # Add imported library have limit scope
        # During the export cmake add library without GLOBAL parameter and no
        # way to change this bihaviour. Let's fix it.
        file (READ ${include_path} _file_content)
        string (REPLACE "IMPORTED)" "IMPORTED GLOBAL)" _file_content "${_file_content}")
        file(WRITE ${include_path} "${_file_content}") 

        include(${include_path})
    endif()
endfunction()

function(find_extproject name)  
    set(options OPTIONAL)
    set(oneValueArgs SHARED)
    set(multiValueArgs CMAKE_ARGS)
    cmake_parse_arguments(find_extproject "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
    
    # set default third party lib path
    if(NOT DEFINED EP_PREFIX)
        set(EP_PREFIX "${CMAKE_BINARY_DIR}/third-party")
    endif()

    # set default url
    if(NOT DEFINED EP_URL)
        set(EP_URL "https://github.com/nextgis-borsch")
    endif()  
    
    if(NOT DEFINED PULL_UPDATE_PERIOD)
        set(PULL_UPDATE_PERIOD 25) # 25 min
    endif()
    
    if(NOT DEFINED PULL_TIMEOUT)
        set(PULL_TIMEOUT 100) # 100 ms
    endif()

    if(NOT DEFINED SUPRESS_VERBOSE_OUTPUT)
        set(SUPRESS_VERBOSE_OUTPUT TRUE)
    endif()

    list(APPEND find_extproject_CMAKE_ARGS -DEP_PREFIX=${EP_PREFIX})   
    list(APPEND find_extproject_CMAKE_ARGS -DEP_URL=${EP_URL})       
    list(APPEND find_extproject_CMAKE_ARGS -DPULL_UPDATE_PERIOD=${PULL_UPDATE_PERIOD})       
    list(APPEND find_extproject_CMAKE_ARGS -DPULL_TIMEOUT=${PULL_TIMEOUT})       
    list(APPEND find_extproject_CMAKE_ARGS -DSUPRESS_VERBOSE_OUTPUT=${SUPRESS_VERBOSE_OUTPUT}) 
    if(CMAKE_TOOLCHAIN_FILE)
        list(APPEND find_extproject_CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE})
    endif()
    if(ANDROID)
        # TODO: do we need more keys?
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_NDK=${ANDROID_NDK})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_ABI=${ANDROID_ABI})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID=ON)
    endif()     
        
    set_property(DIRECTORY PROPERTY "EP_PREFIX" ${EP_PREFIX})
    
    set(EXT_INSTALL_DIR ${EP_PREFIX})
    set(EXT_STAMP_DIR ${EP_PREFIX}/src/${name}_EP-stamp)
    set(EXT_BUILD_DIR ${EP_PREFIX}/src/${name}_EP-build)
        
    # search CMAKE_INSTALL_PREFIX
    string (REGEX MATCHALL "(^|;)-DCMAKE_INSTALL_PREFIX=[A-Za-z0-9_]*" _matchedVars "${find_extproject_CMAKE_ARGS}")    
    list(LENGTH _matchedVars _list_size)    
    if(_list_size EQUAL 0)
        list(APPEND find_extproject_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_DIR})
    endif()
    unset(_matchedVars)
    
    # search BUILD_SHARED_LIBS
    if(NOT DEFINED find_extproject_SHARED)
        list(APPEND find_extproject_CMAKE_ARGS -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS})
        set(find_extproject_SHARED ${BUILD_SHARED_LIBS})
    elseif(find_extproject_SHARED)
        list(APPEND find_extproject_CMAKE_ARGS -DBUILD_SHARED_LIBS=ON)
    else()
        list(APPEND find_extproject_CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF)
    endif()
    
    # set some arguments  
    if(CMAKE_GENERATOR)        
        list(APPEND find_extproject_CMAKE_ARGS -DCMAKE_GENERATOR=${CMAKE_GENERATOR})    
    endif()
    if(CMAKE_BUILD_TYPE)
        list(APPEND find_extproject_CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
    endif()        
    # list(APPEND find_extproject_CMAKE_ARGS -DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES})       
    if(CMAKE_GENERATOR_TOOLSET)
        list(APPEND find_extproject_CMAKE_ARGS -DCMAKE_GENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET})
    endif() 

    get_cmake_property(_variableNames VARIABLES)
    string (REGEX MATCHALL "(^|;)WITH_[A-Za-z0-9_]*" _matchedVars "${_variableNames}") 
    foreach(_variableName ${_matchedVars})
        if(NOT SUPRESS_VERBOSE_OUTPUT)
            message(STATUS "${_variableName}=${${_variableName}}")
        endif()    
        list(APPEND find_extproject_CMAKE_ARGS -D${_variableName}=${${_variableName}})
    endforeach()
    
        
    # get some properties from <cmakemodules>/findext${name}.cmake file
    include(FindExt${name})    
        
    find_exthost_package(Git)
    if(NOT GIT_FOUND)
      message(FATAL_ERROR "git is required")
      return()
    endif()
       
    include(ExternalProject)
   
    # create delete build file script and custom command to periodically execute it
    file(WRITE ${EP_PREFIX}/tmp/${name}_EP-checkupdate.cmake
        "file(TIMESTAMP ${EXT_STAMP_DIR}/${name}_EP-gitpull.txt LAST_PULL \"%y%j%H%M\" UTC)
         if(NOT LAST_PULL)
            set(LAST_PULL 0)
         endif()
         string(TIMESTAMP CURRENT_TIME \"%y%j%H%M\" UTC)
         math(EXPR DIFF_TIME \"\${CURRENT_TIME} - \${LAST_PULL}\")
         if(DIFF_TIME GREATER ${PULL_UPDATE_PERIOD})
            message(STATUS \"Remove ${name}_EP-build\")
            file(REMOVE ${EXT_STAMP_DIR}/${name}_EP-build)
         endif()")
                  
    ExternalProject_Add(${name}_EP
        GIT_REPOSITORY ${EP_URL}/${repo_name}
        CMAKE_ARGS ${find_extproject_CMAKE_ARGS}
        UPDATE_DISCONNECTED 1
    )
    
    add_custom_command(TARGET ${name}_EP PRE_BUILD
               COMMAND ${CMAKE_COMMAND} -P ${EP_PREFIX}/tmp/${name}_EP-checkupdate.cmake
               COMMENT "Check if update needed ..."               
               VERBATIM)
    
    set(RECONFIGURE OFF)
    set(INCLUDE_EXPORT_PATH "${EXT_BUILD_DIR}/${repo_project}-exports.cmake") 

    if(NOT EXISTS "${EP_PREFIX}/src/${name}_EP/.git")
        color_message("Git clone ${repo_name} ...")
        
        set(error_code 1)
        set(number_of_tries 0)
        while(error_code AND number_of_tries LESS 3)
          execute_process(
            COMMAND ${GIT_EXECUTABLE} clone ${EP_URL}/${repo_name} ${name}_EP
            WORKING_DIRECTORY  ${EP_PREFIX}/src
            RESULT_VARIABLE error_code
            )
          math(EXPR number_of_tries "${number_of_tries} + 1")
        endwhile()
           
        if(error_code)   
            message(FATAL_ERROR "Failed to clone repository: ${EP_URL}/${repo_name}")
            return()
        else()
            #execute_process(COMMAND ${GIT_EXECUTABLE} checkout master
            #    WORKING_DIRECTORY  ${EP_PREFIX}/src/${name}_EP)
            file(WRITE ${EXT_STAMP_DIR}/${name}_EP-gitclone-lastrun.txt "")
            #execute_process(COMMAND ${CMAKE_COMMAND} ${EP_PREFIX}/src/${name}_EP
            #    ${find_extproject_CMAKE_ARGS}
            #    WORKING_DIRECTORY ${EXT_BUILD_DIR})
            set(RECONFIGURE ON)
        endif()   
    else() 
        if(EXISTS ${INCLUDE_EXPORT_PATH})
            check_updates(${EXT_STAMP_DIR}/${name}_EP-gitpull.txt ${PULL_UPDATE_PERIOD} CHECK_UPDATES)
        else()
            set(RECONFIGURE ON)
        endif()
        if(CHECK_UPDATES)
            color_message("Git pull ${repo_name} ...")
            execute_process(COMMAND ${GIT_EXECUTABLE} pull
               WORKING_DIRECTORY  ${EP_PREFIX}/src/${name}_EP
               TIMEOUT ${PULL_TIMEOUT} OUTPUT_VARIABLE OUT_STR)
           if(OUT_STR)
                string(FIND ${OUT_STR} "Already up-to-date" STR_POS)
                if(STR_POS LESS 0)
                    file(REMOVE ${EXT_STAMP_DIR}/${name}_EP-build)
                    set(RECONFIGURE ON)  
                endif()
                file(WRITE ${EXT_STAMP_DIR}/${name}_EP-gitpull.txt "")
            endif()
        endif()        
    endif() 

    if(RECONFIGURE)
        color_message("Configure ${repo_name} ...")
        execute_process(COMMAND ${CMAKE_COMMAND} ${EP_PREFIX}/src/${name}_EP
            ${find_extproject_CMAKE_ARGS}
            WORKING_DIRECTORY ${EXT_BUILD_DIR})         
    endif()
    
    if(EXISTS ${INCLUDE_EXPORT_PATH})
        get_imported_targets(${INCLUDE_EXPORT_PATH} IMPORTED_TARGETS)
        string(TOUPPER ${name}_FOUND IS_FOUND)
        set(${IS_FOUND} TRUE PARENT_SCOPE)
           
        #add to list imported
        include_exports_path(${INCLUDE_EXPORT_PATH})
    else()
        message(WARNING "The path ${INCLUDE_EXPORT_PATH} not exist")
        return()
    endif()    
        
    if(EXISTS ${EXT_BUILD_DIR}/ext_options.cmake)         
        include(${EXT_BUILD_DIR}/ext_options.cmake)

        # add include into  ext_options.cmake
        set(WITHOPT "${WITHOPT}include(${EXT_BUILD_DIR}/ext_options.cmake)\n" PARENT_SCOPE)   
       
        foreach(INCLUDE_EXPORT_PATH ${INCLUDE_EXPORTS_PATHS})   
            include_exports_path(${INCLUDE_EXPORT_PATH})
        endforeach()
        unset(INCLUDE_EXPORT_PATH)
    endif()
    
    add_dependencies(${IMPORTED_TARGETS} ${name}_EP)  
    
    set(DEPENDENCY_LIB ${DEPENDENCY_LIB} ${IMPORTED_TARGETS} PARENT_SCOPE) 
    
    set(IMPORTED_TARGET_PATH)
    foreach(IMPORTED_TARGET ${IMPORTED_TARGETS})
        set(IMPORTED_TARGET_PATH ${IMPORTED_TARGET_PATH} $<TARGET_LINKER_FILE:${IMPORTED_TARGET}>) #${IMPORTED_TARGET}
        if(NOT find_extproject_SHARED)
            get_target_property(LINK_INTERFACE_LIBS "${IMPORTED_TARGET}" INTERFACE_LINK_LIBRARIES)
            foreach(LINK_INTERFACE_LIB ${LINK_INTERFACE_LIBS}) 
                if(LINK_INTERFACE_LIB)
                    string(FIND ${LINK_INTERFACE_LIB} "$<TARGET_LINKER_FILE:" POS)
                    if(POS EQUAL -1)
                        set(IMPORTED_TARGET_PATH ${IMPORTED_TARGET_PATH} ${LINK_INTERFACE_LIB})
                    else()    
                        get_target_name(${LINK_INTERFACE_LIB} INTERFACE_TARGET)                
                        if(TARGET ${INTERFACE_TARGET})
                            set(IMPORTED_TARGET_PATH ${IMPORTED_TARGET_PATH} ${LINK_INTERFACE_LIB})
                        else()
                            message(STATUS "NO TARGET ${LINK_INTERFACE_LIB} -> INTERFACE_TARGET ${INTERFACE_TARGET}")
                        endif()
                    endif()
                endif()
            endforeach()
        endif()
    endforeach()
    set(TARGET_LINK_LIB ${TARGET_LINK_LIB} ${IMPORTED_TARGET_PATH} PARENT_SCOPE)
    
    include_directories(${EXT_INSTALL_DIR}/include)
    foreach (inc ${repo_include})
        include_directories(${EXT_INSTALL_DIR}/include/${inc})
    endforeach ()  
        
    if(WIN32)
        set(_INST_ROOT_PATH /)
    else()
        set(_INST_ROOT_PATH ${CMAKE_INSTALL_PREFIX})
    endif()
    
    install( DIRECTORY ${EXT_INSTALL_DIR}/bin
             DESTINATION ${_INST_ROOT_PATH}
             COMPONENT applications)
             
    install( DIRECTORY ${EXT_INSTALL_DIR}/lib
             DESTINATION ${_INST_ROOT_PATH}
             COMPONENT libraries)
             
    install( DIRECTORY ${EXT_INSTALL_DIR}/include
             DESTINATION ${_INST_ROOT_PATH}
             COMPONENT headers)    
                 
    install( DIRECTORY ${EXT_INSTALL_DIR}/share
             DESTINATION ${_INST_ROOT_PATH}
             COMPONENT libraries)                        

    set(EXPORTS_PATHS ${EXPORTS_PATHS} PARENT_SCOPE)
    set(LINK_SEARCH_PATHS ${LINK_SEARCH_PATHS} ${INCLUDE_LINK_SEARCH_PATHS} ${EP_PREFIX}/lib PARENT_SCOPE)
endfunction()
