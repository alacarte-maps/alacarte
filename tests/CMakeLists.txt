cmake_minimum_required( VERSION 2.8.8 )

#set(TEST_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set(AlaCarteStaticDataDirectory "${CMAKE_CURRENT_SOURCE_DIR}/../data")
set(TestStaticDataDirectory     "${CMAKE_CURRENT_SOURCE_DIR}/data")
set(TestDynamicDataDirectory    "${CMAKE_CURRENT_BINARY_DIR}/data")

# We don't want statistics in our unittests.
remove_definitions(-DStatistic_Activated)

# check for local static build gmock directory
SET(GMOCK_FOUND 0)
IF(EXISTS "${PROJECT_SOURCE_DIR}/gmock" AND IS_DIRECTORY "${PROJECT_SOURCE_DIR}/gmock")
    link_directories(${Boost_LIBRARY_DIRS} ${Cairo_LIBRARY_DIRS} "${PROJECT_SOURCE_DIR}/gmock")
    message("gmock dir found\n")
    SET(GMOCK_FOUND 1)
ENDIF(EXISTS "${PROJECT_SOURCE_DIR}/gmock" AND IS_DIRECTORY "${PROJECT_SOURCE_DIR}/gmock")

#
# Sources
#
file(GLOB_RECURSE UnitTests_general_sources     general/*.cpp)
file(GLOB_RECURSE UnitTests_server_sources      server/*.cpp)
file(GLOB_RECURSE UnitTests_mapcss_sources      mapcss/*.cpp)
file(GLOB_RECURSE UnitTests_importer_sources    importer/*.cpp)
file(GLOB_RECURSE UnitTests_utils_sources       utils/*.cpp)
file(GLOB_RECURSE UnitTests_eval_sources        eval/*.cpp)
file(GLOB_RECURSE UnitTests_parser_sources      parser/*.cpp)
file(GLOB_RECURSE UnitTests_renderer_sources    renderer/*.cpp)
file(GLOB_RECURSE UnitTests_shared_sources      shared/*.cpp)

#
# Targets
#
configure_file("testconfig.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/testconfig.hpp")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#
#  Building tests
#  All sources are rebuilt with ALACARTE_TEST (it is used in .cpp files)
#
add_definitions(-DALACARTE_TEST)

add_library(alacarte-obj-test EXCLUDE_FROM_ALL OBJECT ${alacarte_sources})
add_library(server-obj-test EXCLUDE_FROM_ALL OBJECT ${server_sources})
add_library(UnitTests-shared-obj EXCLUDE_FROM_ALL OBJECT ${UnitTests_shared_sources})

add_executable(unitTests_utils
	EXCLUDE_FROM_ALL
	${UnitTests_utils_sources}
	$<TARGET_OBJECTS:UnitTests-shared-obj>
	$<TARGET_OBJECTS:alacarte-obj-test>
)
add_executable(unitTests_parser
	EXCLUDE_FROM_ALL 
	${UnitTests_parser_sources}
	$<TARGET_OBJECTS:UnitTests-shared-obj>
	$<TARGET_OBJECTS:server-obj-test>
	$<TARGET_OBJECTS:alacarte-obj-test>
)
add_executable(unitTests_general
	EXCLUDE_FROM_ALL
	${UnitTests_general_sources}
	$<TARGET_OBJECTS:UnitTests-shared-obj>
	$<TARGET_OBJECTS:server-obj-test>
	$<TARGET_OBJECTS:alacarte-obj-test>
)
add_executable(unitTests_importer
	EXCLUDE_FROM_ALL
	${UnitTests_importer_sources}
	$<TARGET_OBJECTS:UnitTests-shared-obj>
	$<TARGET_OBJECTS:alacarte-obj-test>
)
add_executable(unitTests_server
	EXCLUDE_FROM_ALL
	${UnitTests_server_sources}
	$<TARGET_OBJECTS:UnitTests-shared-obj>
	$<TARGET_OBJECTS:server-obj-test>
	$<TARGET_OBJECTS:alacarte-obj-test>
)

if(GMOCK_FOUND)
	add_executable(unitTests_mapcss
		EXCLUDE_FROM_ALL
		${UnitTests_mapcss_sources}
		$<TARGET_OBJECTS:UnitTests-shared-obj>
		$<TARGET_OBJECTS:server-obj-test>
		$<TARGET_OBJECTS:alacarte-obj-test>
	)
endif(GMOCK_FOUND)

add_executable(unitTests_eval
	EXCLUDE_FROM_ALL
	${UnitTests_eval_sources}
	$<TARGET_OBJECTS:UnitTests-shared-obj>
	$<TARGET_OBJECTS:server-obj-test>
	$<TARGET_OBJECTS:alacarte-obj-test>
)
add_executable(unitTests_renderer
	EXCLUDE_FROM_ALL
	${UnitTests_renderer_sources}
	$<TARGET_OBJECTS:UnitTests-shared-obj>
	$<TARGET_OBJECTS:server-obj-test>
	$<TARGET_OBJECTS:alacarte-obj-test>
)

target_link_libraries(unitTests_utils       ${Boost_LIBRARIES} ${CAIRO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(unitTests_general     ${Boost_LIBRARIES} ${CAIRO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(unitTests_importer    ${Boost_LIBRARIES} ${CAIRO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(unitTests_server      ${Boost_LIBRARIES} ${CAIRO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${SYSTEM_LIBRARIES})
target_link_libraries(unitTests_parser      ${Boost_LIBRARIES} ${CAIRO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${SYSTEM_LIBRARIES})
if(GMOCK_FOUND)
	target_link_libraries(unitTests_mapcss  ${Boost_LIBRARIES} ${CAIRO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} gmock gtest)
endif(GMOCK_FOUND)
target_link_libraries(unitTests_eval        ${Boost_LIBRARIES} ${CAIRO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${SYSTEM_LIBRARIES})
target_link_libraries(unitTests_renderer    ${Boost_LIBRARIES} ${CAIRO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${SYSTEM_LIBRARIES})


set(test_files
	"${TestDynamicDataDirectory}/karlsruhe_big.carte"
	"${TestDynamicDataDirectory}/renderer_test.carte"
	"${TestDynamicDataDirectory}/test_tiles.carte"
)

file(MAKE_DIRECTORY ${TestDynamicDataDirectory})
add_custom_command(
    OUTPUT "${TestDynamicDataDirectory}/karlsruhe_big.carte"
    COMMAND alacarte-maps-importer "${TestStaticDataDirectory}/karlsruhe_big.osm" "${TestDynamicDataDirectory}/karlsruhe_big.carte"
)
add_custom_command(
    OUTPUT "${TestDynamicDataDirectory}/test_tiles.carte"
    COMMAND alacarte-maps-importer "${TestStaticDataDirectory}/test_tiles.osm" "${TestDynamicDataDirectory}/test_tiles.carte"
)
add_custom_command(
    OUTPUT "${TestDynamicDataDirectory}/renderer_test.carte"
    COMMAND alacarte-maps-importer "${TestStaticDataDirectory}/renderer_test.osm" "${TestDynamicDataDirectory}/renderer_test.carte"
)

add_custom_target(tests ALL DEPENDS
	alacarte-maps-importer
	unitTests_server
	unitTests_importer
	unitTests_general
	unitTests_parser
	unitTests_utils
	unitTests_eval
	unitTests_renderer
	${test_files}
)

add_test(unitTests_utils unitTests_utils DEPENDS tests)
add_test(unitTests_eval unitTests_eval)
add_test(unitTests_parser unitTests_parser)
add_test(unitTests_general unitTests_general)
add_test(unitTests_importer unitTests_importer)
#FIXME: failing on windows
if(NOT WIN32)
  add_test(unitTests_server unitTests_server)
endif()
add_test(unitTests_renderer unitTests_renderer)

if(GMOCK_FOUND)
   add_test(unitTests_mapcss unitTests_mapcss)
endif(GMOCK_FOUND)
