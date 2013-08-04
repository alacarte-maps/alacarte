project(alacarte)

cmake_minimum_required( VERSION 2.8 )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Depends
#
find_package(Boost 1.47.0 COMPONENTS system filesystem thread unit_test_framework program_options serialization regex iostreams REQUIRED)

if (NOT Boost_FOUND)
	message(FATAL_ERROR "Fatal error: Boost (version >= 1.47.0) required.\n")
endif (NOT Boost_FOUND)

# Check if boost compiles with libc.
set(CMAKE_REQUIRED_INCLUDES ${Boost_INCLUDE_DIRS})
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
#include <boost/thread/xtime.hpp>
int main(void)
{
	return 0;
}
" BOOST_XTIME)
if (NOT BOOST_XTIME)
	message (FATAL_ERROR "Fatal error: Boost xtime seems to be broken. Consider upgrading Boost to >= 1.50")
endif()

find_package(Log4Cpp REQUIRED)
# FIXME hack for backward compability
if (NOT Log4Cpp_LIBRARIES)
	set (Log4Cpp_LIBRARIES "${LOG4CPP_LIBRARIES}")
	set (Log4Cpp_DEFINITIONS "${LOG4CPP_DEFINITIONS}")
	set (Log4Cpp_INCLUDE_DIRS "${LOG4CPP_INCLUDE_DIRS}")
endif()

IF(NOT MINGW)
	find_package(Inotify REQUIRED)
ENDIF(NOT MINGW)

find_package(PkgConfig)
pkg_check_modules(Cairomm cairomm-1.0)
pkg_check_modules(Cairo cairo)
pkg_check_modules(Freetype freetype2)
pkg_check_modules(SigC++ sigc++-2.0)

find_package(Doxygen)

#
# Build options
#
STRING (REGEX REPLACE "([0-9]+).([0-9]+).([0-9]+)" "\\1" num "${Cairo_VERSION}")
MATH (EXPR cairo_ver "${num}")
STRING (REGEX REPLACE "([0-9]+).([0-9]+).([0-9]+)" "\\2" num "${Cairo_VERSION}")
MATH (EXPR cairo_major "${num}")
STRING (REGEX REPLACE "([0-9]+).([0-9]+).([0-9]+)" "\\3" num "${Cairo_VERSION}")
MATH (EXPR cairo_minor "${num}")
SET(OLD_CAIRO 0)
if (cairo_ver LESS 1 OR cairo_major LESS 12 OR cairo_minor LESS 2)
	SET(OLD_CAIRO 1)
	message (STATUS "${Cairo_VERSION} < 1.12.2: Enabling render lock.")
endif (cairo_ver LESS 1 OR cairo_major LESS 12 OR cairo_minor LESS 2)

# Set windows version for MinGW
IF(MINGW)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0501")
	add_definitions(-D_WIN32_WINNT=0x0501)
	set(SYSTEM_LIBRARIES ws2_32 wsock32)
ELSE(MINGW)
	include_directories(${INOTIFY_INCLUDE_DIRS})
	set(SYSTEM_LIBRARIES ${INOTIFY_LIBRARY})
ENDIF(MINGW)

set(TEST_DIRECTORY "${CMAKE_SOURCE_DIR}/tests")

link_directories(${Boost_LIBRARY_DIRS} ${Cairomm_LIBRARY_DIRS})

SET(GMOCK_FOUND 0)
# check for local static build gmock directory
IF(EXISTS "${PROJECT_SOURCE_DIR}/gmock" AND IS_DIRECTORY "${PROJECT_SOURCE_DIR}/gmock")
	link_directories(${Boost_LIBRARY_DIRS} ${Cairomm_LIBRARY_DIRS} "${PROJECT_SOURCE_DIR}/gmock")
	message("gmock dir found\n")
	SET(GMOCK_FOUND 1)
ENDIF(EXISTS "${PROJECT_SOURCE_DIR}/gmock" AND IS_DIRECTORY "${PROJECT_SOURCE_DIR}/gmock")

include_directories(include ${Boost_INCLUDE_DIRS} ${Log4Cpp_INCLUDE_DIRS} ${SigC++_INCLUDE_DIRS} ${Freetype_INCLUDE_DIRS} ${Cairomm_INCLUDE_DIRS})

ADD_DEFINITIONS("-std=c++0x")
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)
ADD_DEFINITIONS(${Log4Cpp_DEFINITIONS})
ADD_DEFINITIONS(-DStatistic_Activated)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
SET(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ftest-coverage -fprofile-arcs -fprofile-values -fvpt -pg")
MARK_AS_ADVANCED(CMAKE_CXX_FLAGS_PROFILE)
SET( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Profile."
    FORCE )

set(DEBUG_BUILD 0)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(DEBUG_BUILD 1)
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Profile")
	set(DEBUG_BUILD 1)
endif ()
message (STATUS "Enabling debug code: ${DEBUG_BUILD}")

message(STATUS "Default: ${CMAKE_CXX_FLAGS}")
message(STATUS "Debug: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Profile: ${CMAKE_CXX_FLAGS_PROFILE}")
message(STATUS "Release: ${CMAKE_CXX_FLAGS_RELEASE}")

#
# Sources
#
file(GLOB_RECURSE server_sources 				src/server/*.cpp)
file(GLOB_RECURSE importer_sources 				src/importer/*.cpp)
file(GLOB_RECURSE alacarte_sources 				src/general/*.cpp src/utils/*.cpp)
file(GLOB_RECURSE UnitTests_general_sources 	tests/general/*.cpp)
file(GLOB_RECURSE UnitTests_server_sources 		tests/server/*.cpp)
file(GLOB_RECURSE UnitTests_mapcss_sources 		tests/mapcss/*.cpp)
file(GLOB_RECURSE UnitTests_importer_sources 	tests/importer/*.cpp)
file(GLOB_RECURSE UnitTests_utils_sources 		tests/utils/*.cpp)
file(GLOB_RECURSE UnitTests_eval_sources 		tests/eval/*.cpp)
file(GLOB_RECURSE UnitTests_parser_sources 		tests/parser/*.cpp)
file(GLOB_RECURSE UnitTests_shared_sources 		tests/shared/*.cpp)

#
# Targets
#
configure_file("data/config/alacarte.conf.in" "${CMAKE_SOURCE_DIR}/data/config/alacarte.conf")
configure_file("tests/config.hpp.in" "${CMAKE_SOURCE_DIR}/tests/config.hpp")
configure_file("include/config.hpp.in" "${CMAKE_SOURCE_DIR}/include/config.hpp")

add_executable(alacarte-server 		src/alacarte_server.cpp 	${server_sources}	${alacarte_sources})
add_executable(alacarte-importer 	src/alacarte_importer.cpp 	${importer_sources} ${alacarte_sources})
add_executable(unitTests_utils 		EXCLUDE_FROM_ALL ${UnitTests_utils_sources} ${alacarte_sources} ${UnitTests_shared_sources})
add_executable(unitTests_parser		EXCLUDE_FROM_ALL ${UnitTests_parser_sources} ${alacarte_sources} ${server_sources} ${UnitTests_shared_sources})
add_executable(unitTests_general 	EXCLUDE_FROM_ALL ${UnitTests_general_sources} ${alacarte_sources} ${UnitTests_shared_sources})
add_executable(unitTests_importer 	EXCLUDE_FROM_ALL ${UnitTests_importer_sources} ${UnitTests_shared_sources} ${importer_source} ${alacarte_sources})
add_executable(unitTests_server 	EXCLUDE_FROM_ALL ${UnitTests_server_sources} ${UnitTests_shared_sources} ${server_sources} ${alacarte_sources})
IF(GMOCK_FOUND)
	add_executable(unitTests_mapcss 	EXCLUDE_FROM_ALL ${UnitTests_mapcss_sources} ${UnitTests_shared_sources} ${server_sources} ${alacarte_sources})
ENDIF(GMOCK_FOUND)
add_executable(unitTests_eval	 	EXCLUDE_FROM_ALL ${UnitTests_eval_sources} ${UnitTests_shared_sources} ${server_sources} ${alacarte_sources})

set_target_properties(unitTests_eval PROPERTIES COMPILE_DEFINITIONS "ALACARTE_TEST")
set_target_properties(unitTests_general PROPERTIES COMPILE_DEFINITIONS "ALACARTE_TEST")
set_target_properties(unitTests_utils PROPERTIES COMPILE_DEFINITIONS "ALACARTE_TEST")
set_target_properties(unitTests_parser PROPERTIES COMPILE_DEFINITIONS "ALACARTE_TEST")
set_target_properties(unitTests_eval PROPERTIES COMPILE_DEFINITIONS "ALACARTE_TEST")
set_target_properties(unitTests_server PROPERTIES COMPILE_DEFINITIONS "ALACARTE_TEST")
IF(GMOCK_FOUND)
	set_target_properties(unitTests_mapcss PROPERTIES COMPILE_DEFINITIONS "ALACARTE_TEST")
ENDIF(GMOCK_FOUND)

target_link_libraries(alacarte-server 		${Boost_LIBRARIES} ${Cairomm_LIBRARIES} ${Log4Cpp_LIBRARIES} ${SigC++_LIBRARIES} pthread ${SYSTEM_LIBRARIES})
target_link_libraries(alacarte-importer 	${Boost_LIBRARIES} ${Log4Cpp_LIBRARIES} pthread ${SYSTEM_LIBRARIES})
target_link_libraries(unitTests_utils 		${Boost_LIBRARIES} ${Cairomm_LIBRARIES} ${Log4Cpp_LIBRARIES} pthread)
target_link_libraries(unitTests_general 	${Boost_LIBRARIES} ${Cairomm_LIBRARIES} ${Log4Cpp_LIBRARIES} ${SigC++_LIBRARIES} pthread)
target_link_libraries(unitTests_importer 	${Boost_LIBRARIES} ${Log4Cpp_LIBRARIES} ${Cairomm_LIBRARIES} ${SigC++_LIBRARIES} pthread)
target_link_libraries(unitTests_server 		${Boost_LIBRARIES} ${Log4Cpp_LIBRARIES} ${Cairomm_LIBRARIES} ${SigC++_LIBRARIES} pthread)
target_link_libraries(unitTests_parser 		${Boost_LIBRARIES} ${Log4Cpp_LIBRARIES} ${Cairomm_LIBRARIES} ${SigC++_LIBRARIES} pthread)
IF(GMOCK_FOUND)
	target_link_libraries(unitTests_mapcss 		${Boost_LIBRARIES} ${Log4Cpp_LIBRARIES} ${Cairomm_LIBRARIES} ${SigC++_LIBRARIES} pthread gmock gtest)
ENDIF(GMOCK_FOUND)
target_link_libraries(unitTests_eval 		${Boost_LIBRARIES} ${Log4Cpp_LIBRARIES} ${Cairomm_LIBRARIES} ${SigC++_LIBRARIES} pthread)

set(test_files "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/input/karlsruhe_big.carte" "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/input/renderer_test.carte" "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/input/test_tiles.carte")

add_custom_command(
	OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/input/karlsruhe_big.carte"
	COMMAND alacarte-importer "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/input/karlsruhe_big.osm" "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/input/karlsruhe_big.carte"
)
add_custom_command(
	OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/input/test_tiles.carte"
	COMMAND alacarte-importer "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/input/test_tiles.osm" "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/input/test_tiles.carte"
)
add_custom_command(
	OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/input/renderer_test.carte"
	COMMAND alacarte-importer "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/input/renderer_test.osm" "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/input/renderer_test.carte"
)

add_custom_target(tests
	DEPENDS alacarte-importer unitTests_server unitTests_importer unitTests_general unitTests_parser unitTests_utils ${test_files}
)

# add a target to generate API documentation with Doxygen
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc.doxy ${CMAKE_CURRENT_BINARY_DIR}/doc.doxy @ONLY)
	add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc.doxy
		COMMAND convert -density 100 doc/implementierung/gantt.pdf doc/doxygen/gantt.png
		COMMAND xdg-open doc/doxygen/html/index.html
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif(DOXYGEN_FOUND)

#
# Installation
#
install(TARGETS alacarte-server RUNTIME DESTINATION bin)
install(TARGETS alacarte-importer RUNTIME DESTINATION bin)
install(FILES data/config/alacarte.conf DESTINATION /etc)
install(FILES data/default.png DESTINATION share/alacarte)
install(DIRECTORY data/mapcss/ DESTINATION share/alacarte/styles)
install(DIRECTORY data/img DESTINATION share/alacarte)
install(DIRECTORY data/icons DESTINATION share/alacarte)
