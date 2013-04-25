# Find libinotify (inotify compatibility library used on FreeBSD and MacOS X)
#
# INOTIFY_INCLUDE_DIR
# INOTIFY_LIBRARY
# INOTIFY_FOUND
#

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
	# not needed on Linux
	SET(INOTIFY_FOUND TRUE)

	SET(INOTIFY_INCLUDE_DIR "")
	SET(INOTIFY_LIBRARY "")
ELSE(CMAKE_SYSTEM_NAME MATCHES "Linux")
	FIND_PATH(INOTIFY_INCLUDE_DIR NAMES sys/inotify.h)

	FIND_LIBRARY(INOTIFY_LIBRARY NAMES inotify)

	INCLUDE(FindPackageHandleStandardArgs)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(INOTIFY DEFAULT_MSG INOTIFY_LIBRARY INOTIFY_INCLUDE_DIR)
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")

MARK_AS_ADVANCED(INOTIFY_INCLUDE_DIR INOTIFY_LIBRARY)
