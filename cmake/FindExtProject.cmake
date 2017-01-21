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
    set(options OPTIONAL EXACT)
    set(oneValueArgs VERSION SHARED)
    set(multiValueArgs CMAKE_ARGS)
    cmake_parse_arguments(find_extproject "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    # set default third party lib path
    if(NOT DEFINED EP_PREFIX)
        set(EP_PREFIX "${CMAKE_BINARY_DIR}/third-party")
    endif()

    # set default third party build path
    if(NOT DEFINED EXT_BUILD_DIR)
        set(EXT_BUILD_DIR "${EP_PREFIX}/build")
    endif()

    # set default third party tmp path
    if(NOT DEFINED EXT_TMP_DIR)
        set(EXT_TMP_DIR "${EXT_BUILD_DIR}/tmp")
    endif()

    # set default third party download path
    if(NOT DEFINED EXT_DOWNLOAD_DIR)
        set(EXT_DOWNLOAD_DIR "${EP_PREFIX}/src")
    endif()

    # set default third party install path
    if(NOT DEFINED EXT_INSTALL_DIR)
        set(EXT_INSTALL_DIR "${EP_PREFIX}/install")
    endif()

    # set default url
    if(NOT DEFINED EP_URL)
        set(EP_URL "https://github.com/nextgis-borsch")
    endif()

    # set default branch
    if(NOT DEFINED EP_BRANCH)
        set(EP_BRANCH "master")
    endif()

    if(NOT DEFINED PULL_UPDATE_PERIOD)
        set(PULL_UPDATE_PERIOD 25) # 25 min
    endif()

    if(NOT DEFINED PULL_TIMEOUT)
        set(PULL_TIMEOUT 100) # 100 ms
    endif()

    if(NOT DEFINED SUPPRESS_VERBOSE_OUTPUT)
        set(SUPPRESS_VERBOSE_OUTPUT TRUE)
    endif()

    list(APPEND find_extproject_CMAKE_ARGS -DEP_PREFIX=${EP_PREFIX})
    list(APPEND find_extproject_CMAKE_ARGS -DEXT_BUILD_DIR=${EXT_BUILD_DIR})
    list(APPEND find_extproject_CMAKE_ARGS -DEXT_TMP_DIR=${EXT_TMP_DIR})
    list(APPEND find_extproject_CMAKE_ARGS -DEXT_DOWNLOAD_DIR=${EXT_DOWNLOAD_DIR})
    list(APPEND find_extproject_CMAKE_ARGS -DEXT_INSTALL_DIR=${EXT_INSTALL_DIR})
    list(APPEND find_extproject_CMAKE_ARGS -DEP_URL=${EP_URL})
    list(APPEND find_extproject_CMAKE_ARGS -DEP_BRANCH=${EP_BRANCH})
    list(APPEND find_extproject_CMAKE_ARGS -DPULL_UPDATE_PERIOD=${PULL_UPDATE_PERIOD})
    list(APPEND find_extproject_CMAKE_ARGS -DPULL_TIMEOUT=${PULL_TIMEOUT})
    list(APPEND find_extproject_CMAKE_ARGS -DSUPPRESS_VERBOSE_OUTPUT=${SUPPRESS_VERBOSE_OUTPUT})
    if(CMAKE_TOOLCHAIN_FILE)
        list(APPEND find_extproject_CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE})
    endif()
    if(ANDROID)
        # TODO: do we need more keys?
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID=ON)
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_TOOLCHAIN_NAME=${ANDROID_TOOLCHAIN_NAME})

        # Configurable variables from android-sdk/cmake/3.6/android.toolchain.cmake (package version 3.6.3027481)
        # Modeled after the ndk-build system.
        # For any variables defined in:
        #         https://developer.android.com/ndk/guides/android_mk.html
        #         https://developer.android.com/ndk/guides/application_mk.html
        # if it makes sense for CMake, then replace LOCAL, APP, or NDK with ANDROID, and
        # we have that variable below.
        # The exception is ANDROID_TOOLCHAIN vs NDK_TOOLCHAIN_VERSION.
        # Since we only have one version of each gcc and clang, specifying a version
        # doesn't make much sense.
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_NDK=${ANDROID_NDK})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_TOOLCHAIN=${ANDROID_TOOLCHAIN})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_ABI=${ANDROID_ABI})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_PLATFORM=${ANDROID_PLATFORM})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_STL=${ANDROID_STL})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_PIE=${ANDROID_PIE})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_CPP_FEATURES=${ANDROID_CPP_FEATURES})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_ALLOW_UNDEFINED_SYMBOLS=${ANDROID_ALLOW_UNDEFINED_SYMBOLS})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_ARM_MODE=${ANDROID_ARM_MODE})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_ARM_NEON=${ANDROID_ARM_NEON})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_DISABLE_NO_EXECUTE=${ANDROID_DISABLE_NO_EXECUTE})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_DISABLE_RELRO=${ANDROID_DISABLE_RELRO})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_DISABLE_FORMAT_STRING_CHECKS=${ANDROID_DISABLE_FORMAT_STRING_CHECKS})
        list(APPEND find_extproject_CMAKE_ARGS -DANDROID_CCACHE=${ANDROID_CCACHE})
    endif()

    set_property(DIRECTORY PROPERTY "EP_PREFIX" ${EP_PREFIX})

    set(EXT_SOURCE_DIR "${EXT_DOWNLOAD_DIR}/${name}_EP")
    set(EXT_STAMP_DIR "${EXT_BUILD_DIR}/${name}_EP-stamp")
    set(EXT_BINARY_DIR "${EXT_BUILD_DIR}/${name}_EP-build")

    # search CMAKE_INSTALL_PREFIX
    string (REGEX MATCHALL "(^|;)-DCMAKE_INSTALL_PREFIX=[A-Za-z0-9_]*" _matchedVars "${find_extproject_CMAKE_ARGS}")
    list(LENGTH _matchedVars _list_size)
    if(_list_size EQUAL 0)
        list(APPEND find_extproject_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_DIR})
    endif()
    unset(_matchedVars)

    if(OSX_FRAMEWORK)
        list(APPEND find_extproject_CMAKE_ARGS -DOSX_FRAMEWORK=${OSX_FRAMEWORK})
        list(APPEND find_extproject_CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF)
        list(APPEND find_extproject_CMAKE_ARGS -DBUILD_STATIC_LIBS=OFF)
    else()
        # search BUILD_SHARED_LIBS
        if(NOT DEFINED find_extproject_SHARED)
            list(APPEND find_extproject_CMAKE_ARGS -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS})
            set(find_extproject_SHARED ${BUILD_SHARED_LIBS})
        elseif(find_extproject_SHARED)
            list(APPEND find_extproject_CMAKE_ARGS -DBUILD_SHARED_LIBS=ON)
        else()
            list(APPEND find_extproject_CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF)
        endif()
    endif()

    # set some arguments
    if(CMAKE_GENERATOR)
        list(APPEND find_extproject_CMAKE_ARGS -DCMAKE_GENERATOR=${CMAKE_GENERATOR})
    endif()
    if(CMAKE_MAKE_PROGRAM)
        list(APPEND find_extproject_CMAKE_ARGS -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM})
    endif()
    if(CMAKE_BUILD_TYPE)
        list(APPEND find_extproject_CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
    endif()
    # list(APPEND find_extproject_CMAKE_ARGS -DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES})
    if(CMAKE_GENERATOR_TOOLSET)
        list(APPEND find_extproject_CMAKE_ARGS -DCMAKE_GENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET})
    endif()
    if(SKIP_GIT_PULL)
        list(APPEND find_extproject_CMAKE_ARGS -DSKIP_GIT_PULL=${SKIP_GIT_PULL})
    endif()

    get_cmake_property(_variableNames VARIABLES)
    string (REGEX MATCHALL "(^|;)WITH_[A-Za-z0-9_]*" _matchedVars "${_variableNames}")
    foreach(_variableName ${_matchedVars})
        if(NOT SUPPRESS_VERBOSE_OUTPUT)
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
    file(WRITE ${EXT_TMP_DIR}/${name}_EP-checkupdate.cmake
        "file(TIMESTAMP ${EXT_STAMP_DIR}/${name}_EP-gitpull.txt LAST_PULL \"%y%j%H%M\" UTC)
         if(NOT LAST_PULL)
            set(LAST_PULL 0)
         endif()
         string(TIMESTAMP CURRENT_TIME \"%y%j%H%M\" UTC)
         math(EXPR DIFF_TIME \"\${CURRENT_TIME} - \${LAST_PULL}\")
         if(DIFF_TIME GREATER ${PULL_UPDATE_PERIOD})
            message(STATUS \"Git pull ${repo_name} ...\")
            execute_process(COMMAND ${GIT_EXECUTABLE} pull
               WORKING_DIRECTORY  ${EXT_SOURCE_DIR}
               TIMEOUT ${PULL_TIMEOUT} OUTPUT_VARIABLE OUT_STR)
            if(OUT_STR)
                string(FIND \${OUT_STR} \"Already up-to-date\" STR_POS)
                if(STR_POS LESS 0)
                    message(STATUS \"Remove ${name}_EP-build\")
                    file(REMOVE ${EXT_STAMP_DIR}/${name}_EP-build)
                endif()
                file(WRITE ${EXT_STAMP_DIR}/${name}_EP-gitpull.txt \"\")
            endif()
         endif()")

    ExternalProject_Add(${name}_EP
        TMP_DIR ${EXT_TMP_DIR}
        STAMP_DIR ${EXT_STAMP_DIR}
        DOWNLOAD_DIR ${EXT_DOWNLOAD_DIR}
        SOURCE_DIR ${EXT_SOURCE_DIR}
        BINARY_DIR ${EXT_BINARY_DIR}
        INSTALL_DIR ${EXT_INSTALL_DIR}
        GIT_REPOSITORY ${EP_URL}/${repo_name}
        GIT_TAG ${EP_BRANCH}
        CMAKE_ARGS ${find_extproject_CMAKE_ARGS}
        UPDATE_DISCONNECTED 1
    )

    if(NOT SKIP_GIT_PULL)
        add_custom_command(TARGET ${name}_EP PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -P ${EXT_TMP_DIR}/${name}_EP-checkupdate.cmake
                   COMMENT "Check if update needed ..."
                   VERBATIM)
    endif()

    set(RECONFIGURE OFF)
    set(INCLUDE_EXPORT_PATH "${EXT_BINARY_DIR}/${repo_project}-exports.cmake")

    if(NOT EXISTS "${EXT_SOURCE_DIR}/.git")
        color_message("Git clone ${repo_name} ...")

        set(error_code 1)
        set(number_of_tries 0)
        while(error_code AND number_of_tries LESS 3)
          execute_process(
            COMMAND ${GIT_EXECUTABLE} clone --depth 1 ${EP_URL}/${repo_name} ${name}_EP
            WORKING_DIRECTORY  ${EXT_DOWNLOAD_DIR}
            RESULT_VARIABLE error_code
            )
          math(EXPR number_of_tries "${number_of_tries} + 1")
        endwhile()

        if(error_code)
            message(FATAL_ERROR "Failed to clone repository: ${EP_URL}/${repo_name}")
            return()
        else()
            # check version
            if(find_extproject_EXACT)
                set(BRANCH_NAME FALSE)
                execute_process(COMMAND ${GIT_EXECUTABLE} tag -l "v*"
                    OUTPUT_VARIABLE EP_TAGS
                    WORKING_DIRECTORY  ${EXT_SOURCE_DIR})
                string(REPLACE "\n" " " EP_TAGS ${EP_TAGS})
                foreach(EP_TAG ${EP_TAGS})
                    string(SUBSTRING ${EP_TAG} 1 -1 EP_TAG)
                    if(find_extproject_VERSION VERSION_EQUAL EP_TAG)
                        set(BRANCH_NAME "tags/v${EP_TAG}")
                    endif()
                endforeach()
                if(NOT BRANCH_NAME)
                    message(FATAL_ERROR "No ${name} version ${find_extproject_VERSION} exist in remote repository")
                endif()
            else()
                set(BRANCH_NAME ${EP_BRANCH})
            endif()
            # checkout branch
            execute_process(COMMAND ${GIT_EXECUTABLE} checkout ${BRANCH_NAME}
                WORKING_DIRECTORY  ${EXT_SOURCE_DIR})
            file(WRITE ${EXT_STAMP_DIR}/${name}_EP-gitclone-lastrun.txt "")
            #execute_process(COMMAND ${CMAKE_COMMAND} ${EXT_SOURCE_DIR}
            #    ${find_extproject_CMAKE_ARGS}
            #    WORKING_DIRECTORY ${EXT_BINARY_DIR})
            set(RECONFIGURE ON)
        endif()
    else()
        if(EXISTS ${INCLUDE_EXPORT_PATH})
            check_updates(${EXT_STAMP_DIR}/${name}_EP-gitpull.txt ${PULL_UPDATE_PERIOD} CHECK_UPDATES)
        else()
            set(RECONFIGURE ON)
        endif()
        if(CHECK_UPDATES AND NOT SKIP_GIT_PULL)
            color_message("Git pull ${repo_name} ...")
            execute_process(COMMAND ${GIT_EXECUTABLE} pull
               WORKING_DIRECTORY  ${EXT_SOURCE_DIR}
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
        execute_process(COMMAND ${CMAKE_COMMAND} ${EXT_SOURCE_DIR}
            ${find_extproject_CMAKE_ARGS}
            WORKING_DIRECTORY ${EXT_BINARY_DIR})

        # TODO: check exact version if(find_extproject_EXACT)
        # if(find_extproject_VERSION VERSION_EQUAL ... get version from
        # ${name}_EP sources OR find_extproject_VERSION VERSION_LESS)
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

    if(EXISTS ${EXT_BINARY_DIR}/ext_options.cmake)
        include(${EXT_BINARY_DIR}/ext_options.cmake)

        # add include into  ext_options.cmake
        set(WITHOPT "${WITHOPT}include(${EXT_BINARY_DIR}/ext_options.cmake)\n" PARENT_SCOPE)

        foreach(INCLUDE_EXPORT_PATH ${INCLUDE_EXPORTS_PATHS})
            include_exports_path(${INCLUDE_EXPORT_PATH})
        endforeach()
        unset(INCLUDE_EXPORT_PATH)
    endif()

    add_dependencies(${IMPORTED_TARGETS} ${name}_EP)  # FIXME: IMPORTED_TARGETS is list !!!

    set(DEPENDENCY_LIB ${DEPENDENCY_LIB} ${IMPORTED_TARGETS} PARENT_SCOPE)

    set(IMPORTED_TARGET_PATH)

    foreach(IMPORTED_TARGET ${IMPORTED_TARGETS})
        if(repo_header_only)
            continue()
        endif()
        if (repo_intetrface)
            get_target_property(LINK_INTERFACE_LIBS "${IMPORTED_TARGET}" INTERFACE_LINK_LIBRARIES)
            foreach(LINK_INTERFACE_LIB ${LINK_INTERFACE_LIBS})
                if(LINK_INTERFACE_LIB)
                    set(IMPORTED_TARGET_PATH ${IMPORTED_TARGET_PATH} ${LINK_INTERFACE_LIB})
                endif()
            endforeach()
            get_target_property(INTERFACE_COMPILE_DEFINITIONS "${IMPORTED_TARGET}" INTERFACE_COMPILE_DEFINITIONS)
            foreach(INTERFACE_COMPILE_DEFINITION ${INTERFACE_COMPILE_DEFINITIONS})
                add_definitions(-D${INTERFACE_COMPILE_DEFINITION})
            endforeach()
        else()
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
        endif()
    endforeach()
    set(TARGET_LINK_LIB ${TARGET_LINK_LIB} ${IMPORTED_TARGET_PATH} PARENT_SCOPE)

    if(OSX_FRAMEWORK)
        set(EXT_HEADERS_SUFFIX "Library/Frameworks/${repo_project}.framework/Headers")
    else()
        set(EXT_HEADERS_SUFFIX "include")
    endif()

    include_directories(${EXT_INSTALL_DIR}/${EXT_HEADERS_SUFFIX})
    foreach (inc ${repo_include})
        include_directories(${EXT_INSTALL_DIR}/${EXT_HEADERS_SUFFIX}/${inc})
    endforeach ()

    if(WIN32)
        set(_INST_ROOT_PATH /)
    else()
        set(_INST_ROOT_PATH ${CMAKE_INSTALL_PREFIX})
    endif()

    if(OSX_FRAMEWORK)
        file(MAKE_DIRECTORY "${EXT_INSTALL_DIR}/Applications")
        file(MAKE_DIRECTORY "${EXT_INSTALL_DIR}/Library")

        install( DIRECTORY ${EXT_INSTALL_DIR}/Applications
                 DESTINATION ${_INST_ROOT_PATH}
                 COMPONENT applications)

        install( DIRECTORY ${EXT_INSTALL_DIR}/Library
                 DESTINATION ${_INST_ROOT_PATH}
                 COMPONENT libraries)
    else()
        # create directories
        file(MAKE_DIRECTORY "${EXT_INSTALL_DIR}/bin")
        file(MAKE_DIRECTORY "${EXT_INSTALL_DIR}/lib")
        file(MAKE_DIRECTORY "${EXT_INSTALL_DIR}/include")
        file(MAKE_DIRECTORY "${EXT_INSTALL_DIR}/share")

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
    endif()

    set(EXPORTS_PATHS ${EXPORTS_PATHS} PARENT_SCOPE)
    set(LINK_SEARCH_PATHS ${LINK_SEARCH_PATHS} ${INCLUDE_LINK_SEARCH_PATHS} ${EP_PREFIX}/install/lib PARENT_SCOPE)
endfunction()