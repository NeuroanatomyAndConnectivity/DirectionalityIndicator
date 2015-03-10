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
# along with DirectionalityIndicator. If not, see <http://www.gnu.org/licenses/>.
#
#----------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# External Code Setup (/ext)
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Add /ext directory to include search path
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/ext" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Setup GLM
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Is header-only. Use ext as include search path (see above)
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/ext" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Setup GLEW
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Include correct path to ensure that the usual #include <GL/glew.h> is working properly
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/ext/glew" )
# Build the code below. Just store path here.
SET( TARGET_EXT_GLEW_C_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ext/glew/glew.c" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Setup RPly
# ---------------------------------------------------------------------------------------------------------------------------------------------------

FILE( GLOB_RECURSE TARGET_EXT_RPLY_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/ext/rply/*.c )

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
# Setup Shader Stuff
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Use the above utility code to setup the GFX core shaders.
COLLECT_SHADER_FILES( ${CMAKE_CURRENT_SOURCE_DIR}/gfx/shaders GFX_GLSL_FILES )
SETUP_SHADERS( "${GFX_GLSL_FILES}" "gfx/shaders" )

# Also setup algorithm shaders.
COLLECT_SHADER_FILES( ${CMAKE_CURRENT_SOURCE_DIR}/algorithms ALGORITHM_GLSL_FILES )
SETUP_SHADERS( "${ALGORITHM_GLSL_FILES}" "algorithms/shaders" )

# Use src/ as include search path
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Build the binary
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# How to call the binary?
SET( BinName "di" )

# Setup the target
# ADD_EXECUTABLE( ${BinName} ${TARGET_CPP_FILES} ${TARGET_H_FILES} ${TARGET_EXT_RPLY_CPP_FILES} )
ADD_LIBRARY( ${BinName} SHARED ${TARGET_CPP_FILES} ${TARGET_H_FILES} ${TARGET_EXT_RPLY_CPP_FILES} ${TARGET_EXT_GLEW_C_FILES} )

# Some Linux distributions need to explicitly link against X11. We add this lib here.
IF( CMAKE_HOST_SYSTEM MATCHES "Linux" )
    SET( ADDITIONAL_TARGET_LINK_LIBRARIES "X11" )
ENDIF()
TARGET_LINK_LIBRARIES( ${BinName} ${CMAKE_STANDARD_LIBRARIES}
                                  ${OPENGL_LIBRARIES}
                                  ${GLEW_LIBRARIES}
                                  Qt5::Widgets Qt5::OpenGL Qt5::WebKitWidgets 
                                  ${ADDITIONAL_TARGET_LINK_LIBRARIES} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Style
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# setup the stylechecker. Ignore the platform specific stuff.
SETUP_STYLECHECKER( "${BinName}"
                    "${TARGET_CPP_FILES};${TARGET_H_FILES}"  # add all these files to the stylechecker
                    "ext/*;io/PlyReader.cpp" )                                # exclude some ugly files
