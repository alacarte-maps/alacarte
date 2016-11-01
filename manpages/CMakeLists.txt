find_program(A2X_EXECUTABLE NAMES a2x a2x.py)

if (A2X_EXECUTABLE)
	set(MAN_NAMES alacarte-maps-importer.1 alacarte-maps-server.1)

	set(ZIPPED_MAN_FILES)
	set(HTML_MAN_FILES)

	foreach(name IN LISTS MAN_NAMES)
		configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/${name}.adoc.in" "${CMAKE_CURRENT_BINARY_DIR}/${name}.adoc")
		set(man_source   ${CMAKE_CURRENT_BINARY_DIR}/${name}.adoc)
		set(man_html     ${CMAKE_CURRENT_BINARY_DIR}/${name}.html)
		set(man_compiled ${CMAKE_CURRENT_BINARY_DIR}/${name})
		set(man_zipped   ${CMAKE_CURRENT_BINARY_DIR}/${name}.gz)

		add_custom_command(
			OUTPUT  ${man_html}
			COMMAND ${A2X_EXECUTABLE} --format xhtml ${man_source}
			DEPENDS ${man_source}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Generating man page (html version): ${name}.html"
			VERBATIM
		)

		add_custom_command(
			OUTPUT  ${man_compiled}
			COMMAND ${A2X_EXECUTABLE} --format manpage ${man_source}
			DEPENDS ${man_source}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Generating man page: ${name}"
			VERBATIM
		)

		add_custom_command(
			OUTPUT ${man_zipped}
			COMMAND ${CMAKE_COMMAND} -E tar cfz ${man_zipped} ${man_compiled}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			DEPENDS ${man_compiled}
			COMMENT "Compressing man page: ${name}.gz"
			VERBATIM
		)

		list(APPEND ZIPPED_MAN_FILES ${man_zipped})
		list(APPEND HTML_MAN_FILES ${man_html})
	endforeach()

	add_custom_target(man ALL DEPENDS ${HTML_MAN_FILES} ${ZIPPED_MAN_FILES})

	install(FILES ${ZIPPED_MAN_FILES} DESTINATION share/man/man1)
else()
	message(STATUS "Man pages will not be generated. AsciiDoc (a2x) not found.")
endif()
