# - Try to find Log4Cpp
# Once done this will define
#
#  LOG4CPP_FOUND - system has log4cpp
#  LOG4CPP_INCLUDE_DIRS - the log4cpp include directory
#  LOG4CPP_LIBRARIES - Link these to use log4cpp
#  LOG4CPP_DEFINITIONS - Compiler switches required for using log4cpp
#
#  Copyright (c) 2010 Roman Putanowicz <putanowr@l5.pk.edu.pl>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if (LOG4CPP_LIBRARIES AND LOG4CPP_INCLUDE_DIRS)
    set(LOG4CPP_FOUND TRUE) # in cache already
else (LOG4CPP_LIBRARIES AND LOG4CPP_INCLUDE_DIRS)
    find_path(LOG4CPP_INCLUDE_DIR 
        NAMES 
            log4cpp/Appender.hh
        PATHS 
            "${BASE_DIR}/include"
            NO_DEFAULT_PATH
    )
    find_path(LOG4CPP_INCLUDE_DIR 
        NAMES
            log4cpp/Appender.hh
    )

    find_library(LOG4CPP_LIBRARY NAMES log4cpp PATHS "${BASE_DIR_LIB}" NO_DEFAULT_PATH )
    find_library(LOG4CPP_LIBRARY NAMES log4cpp PATHS "${BASE_DIR}/lib" NO_DEFAULT_PATH )
    find_library(LOG4CPP_LIBRARY NAMES log4cpp )

    set(LOG4CPP_INCLUDE_DIRS
    ${LOG4CPP_INCLUDE_DIR} CACHE PATH "Path to log4cpp headers")

    set(LOG4CPP_LIBRARIES
    ${LOG4CPP_LIBRARY} CACHE STRING "Directories to be linked to use log4cpp")

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(log4cpp  DEFAULT_MSG
        LOG4CPP_LIBRARY LOG4CPP_INCLUDE_DIR
    )

    if (LOG4CPP_FOUND)
        set(LOG4CPP_FLAGS "-DHAS_LOG4CPP" CACHE STRING "Macro to switch log4cpp support")
    endif (LOG4CPP_FOUND)

    mark_as_advanced(LOG4CPP_FLAGS LOG4CPP_INCLUDE_DIRS LOG4CPP_INCLUDE_DIR LOG4CPP_LIBRARY LOG4CPP_LIBRARIES)
endif (LOG4CPP_LIBRARIES AND LOG4CPP_INCLUDE_DIRS)