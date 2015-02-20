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

# How to call the binary?
SET( BinName "DirectionalityIndicator" )

# Setup the target
ADD_EXECUTABLE( ${BinName} ${TARGET_CPP_FILES} ${TARGET_H_FILES} )

# Some Linux distributions need to explicitly link against X11. We add this lib here.
IF( CMAKE_HOST_SYSTEM MATCHES "Linux" )
    SET( ADDITIONAL_TARGET_LINK_LIBRARIES "X11" )
ENDIF()
TARGET_LINK_LIBRARIES( ${BinName} "di"
                                  ${CMAKE_STANDARD_LIBRARIES}
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
                    "ext/*" )                                # exclude some ugly files
