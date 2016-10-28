find_program(A2X_EXECUTABLE NAMES a2x a2x.py)
find_program(GZIP_EXECUTABLE NAMES gzip)

set(MAN_NAMES alacarte-maps-importer.1 alacarte-maps-server.1)

set(ZIPPED_MAN_FILES)
set(HTML_MAN_FILES)

file(COPY ${CMAKE_SOURCE_DIR}/manpages DESTINATION ${CMAKE_BINARY_DIR})

foreach(name IN LISTS MAN_NAMES)
	configure_file("${CMAKE_BINARY_DIR}/manpages/${name}.adoc.in" "${CMAKE_BINARY_DIR}/manpages/${name}.adoc")
	set(man_source ${CMAKE_BINARY_DIR}/manpages/${name}.adoc)
	set(man_html ${CMAKE_BINARY_DIR}/manpages/${name}.html)
	set(man_compiled ${CMAKE_BINARY_DIR}/manpages/${name})
	set(man_zipped ${CMAKE_BINARY_DIR}/manpages/${name}.gz)
	add_custom_command(OUTPUT ${man_compiled}
		COMMAND ${A2X_EXECUTABLE} --format manpage ${man_source}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Building manpage ${man_compiled}"
		VERBATIM)
	add_custom_command(OUTPUT ${man_html}
		COMMAND ${A2X_EXECUTABLE} --format xhtml ${man_source}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Building manpage ${man_html}"
		VERBATIM)
	add_custom_command(OUTPUT ${man_zipped}
		COMMAND ${GZIP_EXECUTABLE} ${man_compiled}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		DEPENDS ${man_compiled}
		COMMENT "Compressing manpage ${man_compiled}"
		VERBATIM)
	list(APPEND ZIPPED_MAN_FILES ${man_zipped})
	list(APPEND HTML_MAN_FILES ${man_html})
endforeach()

add_custom_target(man DEPENDS ${ZIPPED_MAN_FILES} ${HTML_MAN_FILES})

install(FILES ${ZIPPED_MAN_FILES} DESTINATION share/man/man1)
