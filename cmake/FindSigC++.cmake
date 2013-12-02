# - Try to find SIGC++
# Once done this will define
#
#  SIGC++_ROOT_DIR - Set this variable to the root installation of SIGC++
#  SIGC++_FOUND - system has SIGC++
#  SIGC++_INCLUDE_DIRS - the SIGC++ include directory
#  SIGC++_LIBRARIES - Link these to use SIGC++
#
#  Copyright (c) 2008 Joshua L. Blocher <verbalshadow at gmail dot com>
#  Copyright (c) 2012 Dmitry Baryshnikov <polimax at mail dot ru>
#  Copyright (c) 2013 Michael Pavlyshko <pavlushko at tut dot by>
#
# Distributed under the OSI-approved BSD License
#

if (NOT WIN32)
    find_package(PkgConfig)
    if (PKG_CONFIG_FOUND)
        pkg_check_modules(_SIGC++ sigc++-2.0)
    endif (PKG_CONFIG_FOUND)
endif (NOT WIN32)

SET(_SIGC++_ROOT_HINTS
    $ENV{SIGC++}
    ${CMAKE_FIND_ROOT_PATH}
    ${SIGC++_ROOT_DIR}
) 

SET(_SIGC++_ROOT_PATHS
    $ENV{SIGC++}/src
    /usr
    /usr/local
)

SET(_SIGC++_ROOT_HINTS_AND_PATHS
    HINTS ${_SIGC++_ROOT_HINTS}
    PATHS ${_SIGC++_ROOT_PATHS}
)

FIND_PATH(SIGC++_INCLUDE_DIR
    NAMES
        "sigc++/sigc++.h"
    HINTS
        ${_SIGC++_INCLUDEDIR}
        ${_SIGC++_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES
        include
        "include/sigc++-2.0"
)  

find_path(SIGC++_CONFIG_INCLUDE_DIR
    NAMES
        sigc++config.h
    HINTS
        ${_SIGC++_LIBDIR}
        ${_SIGC++_INCLUDEDIR}
        ${_SIGC++_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES
        include
        lib
        "sigc++-2.0/include"
        "lib/sigc++-2.0"
        "lib/sigc++-2.0/include"
)

FIND_LIBRARY(SIGC++_LIBRARY
    NAMES
        sigc-2.0
    HINTS
        ${_SIGC++_LIBDIR}
        ${_SIGC++_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES
        "lib"
        "local/lib"
) 

SET(SIGC++_LIBRARIES 
    ${SIGC++_LIBRARY}
)

SET(SIGC++_INCLUDE_DIRS
    ${SIGC++_INCLUDE_DIR}
    ${SIGC++_CONFIG_INCLUDE_DIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SIGC++ "Could NOT find SIGC++, try to set the path to SIGC++ root folder in the system variable SIGC++"
    SIGC++_LIBRARIES
    SIGC++_INCLUDE_DIRS
)

MARK_AS_ADVANCED(SIGC++_CONFIG_INCLUDE_DIR SIGC++_INCLUDE_DIR SIGC++_INCLUDE_DIRS SIGC++_LIBRARY SIGC++_LIBRARIES)