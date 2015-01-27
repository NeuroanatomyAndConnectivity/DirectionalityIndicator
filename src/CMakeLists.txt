# To compile: 
# cmake -L/usr/local/opt/qt5/lib -I/usr/local/opt/qt5/include -L/usr/local/Cellar/qt5/5.3.2/lib/cmake/Qt5Widgets/ src/* -o src/
# make
#
#----------------------------------------------------------------------------------------
#
# Project: DirectionalityIndicator
#
# Copyright 2014-2015 Sebastian Eichelbaum (http://www.sebastian-eichelbaum.de)
#           2014-2015 Max Planck Research Group "Neuroanatomy and Connectivity"
#
# This file is part of DirectionalityIndicator.
#
# DirectionalityIndicator is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DirectionalityIndicator is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with DirectionalityIndicator. If not, see <http:#www.gnu.org/licenses/>.
#
#----------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# General CMake Setup
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( DirectionalityIndicator )

# How to call the binary?
SET( BinName "DirectionalityIndicator" )

# guard against in-source builds
IF( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  MESSAGE( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there (you may need to remove CMakeCache.txt)." )
ENDIF()

# the build types
IF( NOT CMAKE_BUILD_TYPE )
    SET( CMAKE_BUILD_TYPE Release
         CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo"
         FORCE
       )
ENDIF( NOT CMAKE_BUILD_TYPE )

# guard against typos in build-type strings
STRING( TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_tolower)
IF( NOT cmake_build_type_tolower STREQUAL "debug" AND
    NOT cmake_build_type_tolower STREQUAL "release" AND
    NOT cmake_build_type_tolower STREQUAL "relwithdebinfo" AND
    NOT cmake_build_type_tolower STREQUAL "")
  MESSAGE( SEND_ERROR "Unknown build type \"${CMAKE_BUILD_TYPE}\". Allowed values are Debug, Release, RelWithDebInfo  and \"\" (case-insensitive).")
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Compiler Setup
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# GCC 4.7 requires us to explicitly link against libstdc++ and libm. CMake offers a variable for this called "CMAKE_STANDARD_LIBRARIES".
# Unfortunately, this variable is empty. We fill it here and hopefully this is fixed in the near future.
LIST( APPEND CMAKE_STANDARD_LIBRARIES "stdc++" "m" )

# Force latest standard and activate nearly all warnings
SET( CMAKE_CXX_FLAGS "-frtti -pedantic -std=c++11 -Wall -Wno-long-long -Wextra -pthread" CACHE STRING "" FORCE )

# Darwin's ld isn't GNU and doesn't like the following
IF( NOT CMAKE_SYSTEM_NAME MATCHES "Darwin" )
    ## The following allows us to prevent cyclic dependencies even on linux
    SET( CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined -Wl,--allow-shlib-undefined,--as-needed" CACHE STRING "" FORCE )
    SET( CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed" CACHE STRING "" FORCE )
ENDIF()
SET( CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE STRING "" FORCE )
SET( CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -O0" CACHE STRING "" FORCE )

# Supress compilation warnings from includes residing in system paths, see #230 for further details.
SET( CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem" CACHE STRING "" FORCE )

# Export compile commands. Useful if you are using a code completion tool like YouCompleteMe.
SET( CMAKE_EXPORT_COMPILE_COMMAND ON FORCE )

# Use src/ as include search path
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Documentation
#  - Call doxygen here
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# where to find the doxygen config
SET( NL_DOXYGEN_DIR ${PROJECT_SOURCE_DIR}/../doc/developer )

IF( EXISTS ${NL_DOXYGEN_DIR}/doxygenConfig )
    #let doxygen do the work
    ADD_CUSTOM_TARGET( doc
                       COMMAND doxygen ${NL_DOXYGEN_DIR}/doxygenConfig
                       WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
                       COMMENT "Build doxygen documentation"
                       VERBATIM
                     )
ENDIF()

# On Windows, we want the common doc (README, AUTHORS and COPYING) to be in the root install dir too:
IF( CMAKE_HOST_SYSTEM MATCHES "Windows" )
    SETUP_COMMON_DOC( "." "COMMON_DOC_ON_WINDOWS" )
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Style
#  - We use brainlint for this
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Sets up the stylecheck mechanism. Use this to add your codes to the stylecheck mechanism.
# _TargetName the name of the target which gets added here
# _CheckFiles the list of files to check
# _Excludes a list of exclusion rules. These are regular expressions.
FUNCTION( SETUP_STYLECHECKER _TargetName _CheckFiles _Excludes )
    # to exlude some files, check each file against each exlusion rule
    FOREACH( filename ${_CheckFiles} )
        FOREACH( excludeRule ${_Excludes} )
            STRING( REGEX MATCH "${excludeRule}" IsExcluded "${filename}" )
            IF( IsExcluded )
                LIST( REMOVE_ITEM _CheckFiles ${filename} )
            ENDIF( IsExcluded )
        ENDFOREACH( excludeRule )
    ENDFOREACH( filename )

    # the stylechecker allows coloring the output. Enable if color make is active
    IF( CMAKE_COLOR_MAKEFILE )
        SET( STYLECHECK_OPTIONS "--color" )
    ELSE()
        SET( STYLECHECK_OPTIONS "" )
    ENDIF()

    # Further system specific options
    IF( CMAKE_HOST_WIN32 )
        SET( XARGS_OPTIONS "-n 128" )
    ELSEIF( CMAKE_HOST_UNIX )
        SET( XARGS_OPTIONS "-n 64" )
    ELSE()
        SET( XARGS_OPTIONS "" )
    ENDIF()

    # Export our filtered file list to a file in build dir
    SET( BrainLinterListFile "${PROJECT_BINARY_DIR}/brainlint/brainlintlist_${_TargetName}" )
    LIST( REMOVE_ITEM _CheckFiles "" )
    STRING( REPLACE ";" "\n" _CheckFiles "${_CheckFiles}" )
    FILE( WRITE ${BrainLinterListFile} "${_CheckFiles}\n" )

    # add a new target for this lib
    ADD_CUSTOM_TARGET( stylecheck_${_TargetName}
                       COMMAND  cat ${BrainLinterListFile} | xargs ${XARGS_OPTIONS} ${PROJECT_SOURCE_DIR}/../tools/style/brainlint/brainlint.py ${STYLECHECK_OPTIONS} 2>&1 | grep -iv 'Total errors found: 0$$' | cat
                       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                       COMMENT "Check if ${_TargetName} complies to CodingStandard"
    )

    # make the stylecheck taret depend upon this
   ADD_DEPENDENCIES( stylecheck "stylecheck_${_TargetName}" )
ENDFUNCTION( SETUP_STYLECHECKER )

# SETUP_STYLECHECKER adds targets for each lib which then add theirself to this target
ADD_CUSTOM_TARGET( stylecheck )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# REQUIRED third party libs
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Setup QT5
# ---------------------------------------------------------------------------------------------------------------------------------------------------
set (CMAKE_PREFIX_PATH "/usr/local/Cellar/qt5/5.3.2/")

# Package dependencies:
FIND_PACKAGE( Qt5Widgets REQUIRED )
FIND_PACKAGE( Qt5OpenGL REQUIRED )
FIND_PACKAGE( Qt5WebKitWidgets REQUIRED )

# Includes:
INCLUDE_DIRECTORIES( SYSTEM ${QT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( SYSTEM ${Qt5Widgets_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( SYSTEM ${Qt5OpenGL_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( SYSTEM ${Qt5WebKitWidgets_INCLUDE_DIRS} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# GUI compilation options
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Compiling with Qt5 requires some special definitions and flags to be set.

# Collect and set definitions
SET( _QT5_DEFINITIONS "" )
LIST( APPEND _QT5_DEFINITIONS ${Qt5Widgets_DEFINITIONS} )
LIST( APPEND _QT5_DEFINITIONS ${Qt5OpenGL_DEFINITIONS} )
LIST( APPEND _QT5_DEFINITIONS ${Qt5WebKitWidgets_DEFINITIONS} )
LIST( REMOVE_DUPLICATES _QT5_DEFINITIONS )
ADD_DEFINITIONS( ${_QT5_DEFINITIONS} )

# Collect and set compiler flags
SET( _QT5_EXECUTABLE_COMPILE_FLAGS "" )
LIST( APPEND _QT5_EXECUTABLE_COMPILE_FLAGS ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} )
LIST( APPEND _QT5_EXECUTABLE_COMPILE_FLAGS ${Qt5OpenGL_EXECUTABLE_COMPILE_FLAGS} )
LIST( APPEND _QT5_EXECUTABLE_COMPILE_FLAGS ${Qt5WebKitWidgets_EXECUTABLE_COMPILE_FLAGS} )
LIST( REMOVE_DUPLICATES _QT5_EXECUTABLE_COMPILE_FLAGS )
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_QT5_EXECUTABLE_COMPILE_FLAGS}" )

# This is needed since the mocs will be generated there
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# Code Setup
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Collect everything to compile
# ---------------------------------------------------------------------------------------------------------------------------------------------------

FILE( GLOB_RECURSE TARGET_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
FILE( GLOB_RECURSE TARGET_H_FILES   ${CMAKE_CURRENT_SOURCE_DIR}/*.h )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Build the binary
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Qt5 requires all classes with a QWidget stuff inside to be put into the MOC mechanism. We utilize the automoc mechanism here.
SET( CMAKE_AUTOMOC ON )

# Setup the target
ADD_EXECUTABLE( ${BinName} ${TARGET_CPP_FILES} ${TARGET_H_FILES} )

# Some Linux distributions need to explicitly link against X11. We add this lib here.
IF( CMAKE_HOST_SYSTEM MATCHES "Linux" )
    SET( ADDITIONAL_TARGET_LINK_LIBRARIES "X11" )
ENDIF()
TARGET_LINK_LIBRARIES( ${BinName} ${CMAKE_STANDARD_LIBRARIES}
                                  Qt5::Widgets Qt5::OpenGL Qt5::WebKitWidgets 
                                  ${ADDITIONAL_TARGET_LINK_LIBRARIES} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Style
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# setup the stylechecker. Ignore the platform specific stuff.
SETUP_STYLECHECKER( "${BinName}"
                    "${TARGET_CPP_FILES};${TARGET_H_FILES}"  # add all these files to the stylechecker
                    "" )                                     # exlude some ugly files
