# © 2023 Friendly Shade, Inc.
# © 2023 Sebastian Zapata
#
# This file is part of FileHauler.
# FileHauler is licensed under the GNU General Public License (GPL), Version 3. If a copy of the GPL
# was not distributed with this file, you can obtain one at https://www.gnu.org/licenses/gpl-3.0.

# =============================================================================================================
# Add executable
# =============================================================================================================
function(helper_add_executable EXECUTABLE_NAME)

	# Define options
    set(options "")
	
	# Define one value arguments
    set(oneValueArgs FOLDER OUTPUT_NAME)
	
	# Define multiple value arguments
    set(multiValueArgs SOURCES LINKS)
	
	# Parse arguments
    cmake_parse_arguments(EXECUTABLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	# Make sure there's a name for the executable
	if(NOT EXECUTABLE_NAME)
		message(FATAL_ERROR "You must provide a name for the executable")
	endif()

	# Make sure there's a name for the library
	if(NOT EXECUTABLE_OUTPUT_NAME)
		message(FATAL_ERROR "You must provide an output name for the library")
	endif()
	
	# Make sure there's at least one source file for the executable
	LIST(LENGTH EXECUTABLE_SOURCES EXECUTABLE_SOURCES_LENGTH)
	if(EXECUTABLE_SOURCES_LENGTH EQUAL 0)
		message(FATAL_ERROR "You must provide at least one source file for the executable")
	endif()

	# Generate Windows versioning information
	if(MSVC)
		set(INSTALL_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
		set(INSTALL_VERSION_MINOR ${PROJECT_VERSION_MINOR})
		set(INSTALL_VERSION_PATCH ${PROJECT_VERSION_PATCH})
		set(INSTALL_NAME ${EXECUTABLE_OUTPUT_NAME})
		set(INSTALL_PROJECT_NAME ${PROJECT_NAME})
		configure_file(
			"${CMAKE_SOURCE_DIR}/cmake/version.rc.in"
			"${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}_version.rc"
			@ONLY
		)
		list(APPEND EXECUTABLE_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}_version.rc")
	endif()

	# Add executable
	add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SOURCES})

	# Set custom output filename
	set_target_properties(${EXECUTABLE_NAME} PROPERTIES OUTPUT_NAME "${EXECUTABLE_OUTPUT_NAME}")

	# Link to other modules/libraries
	target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${EXECUTABLE_LINKS})

	# Set project filter
	if (EXECUTABLE_FOLDER AND NOT FOLDER STREQUAL "")
		set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER ${EXECUTABLE_FOLDER})
	endif()

	# Set the output directory for the executable
	set_target_properties(${EXECUTABLE_NAME} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	)

	# Install the executable
	install(TARGETS ${EXECUTABLE_NAME}
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}/$<CONFIG>"
	)
	
	set(LIBRARY_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/thirdparty/tclap/include")
	include_directories(${LIBRARY_NAME} ${LIBRARY_INCLUDE_DIRECTORIES})

endfunction()

# =============================================================================================================
# Copy linked targets recursively so the entire dependency tree files are copied to the build folder
# =============================================================================================================
function(copy_dependencies_to_build_directory_recursively TARGET_NAME)
	get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)
	# Only install if it's not an interface. Otherwise we'll get the "target is not an executable or library" error.
	if (NOT ${TARGET_TYPE} STREQUAL "INTERFACE_LIBRARY")
		install(FILES
			$<TARGET_FILE:${TARGET_NAME}>
			DESTINATION "${CMAKE_INSTALL_BINDIR}/$<CONFIG>"
		)

		get_target_property(DEPENDENCIES "${TARGET_NAME}" INTERFACE_LINK_LIBRARIES)
		if (DEPENDENCIES)
			foreach(DEPENDENCY ${DEPENDENCIES})
				copy_dependencies_to_build_directory_recursively(${DEPENDENCY})
			endforeach()
		endif()
	endif()
endfunction()

# =============================================================================================================
# Get version of project
# =============================================================================================================
function(get_project_version OUTPUT_VERSION OUTPUT_VERSION_MAJOR OUTPUT_VERSION_MINOR OUTPUT_VERSION_PATCH)
	file(STRINGS "modules/version.hpp" _FILEHAULER_VERSION_HPP_CONTENTS REGEX "#define FILEHAULER_VERSION_")
	foreach(v MAJOR MINOR PATCH)
		if("${_FILEHAULER_VERSION_HPP_CONTENTS}" MATCHES "#define FILEHAULER_VERSION_${v} ([0-9]+)")
			set(FILEHAULER_VERSION_${v} "${CMAKE_MATCH_1}")
		else()
			message(FATAL_ERROR "Failed to retrieve the FileHauler version from the source code. Missing FILEHAULER_VERSION_${v}.")
		endif()
	endforeach()

	set(${OUTPUT_VERSION} "${FILEHAULER_VERSION_MAJOR}.${FILEHAULER_VERSION_MINOR}.${FILEHAULER_VERSION_PATCH}" PARENT_SCOPE)
	set(${OUTPUT_VERSION_MAJOR} "${FILEHAULER_VERSION_MAJOR}" PARENT_SCOPE)
	set(${OUTPUT_VERSION_MINOR} "${FILEHAULER_VERSION_MINOR}" PARENT_SCOPE)
	set(${OUTPUT_VERSION_PATCH} "${FILEHAULER_VERSION_PATCH}" PARENT_SCOPE)

endfunction()