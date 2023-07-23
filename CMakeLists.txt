# © 2023 Friendly Shade, Inc.
# © 2023 Sebastian Zapata
#
# This file is part of FileHauler.
# FileHauler is licensed under the GNU General Public License (GPL), Version 3. If a copy of the GPL
# was not distributed with this file, you can obtain one at https://www.gnu.org/licenses/gpl-3.0.


# -------------------------------------------------------------------------------------------------
# CMake minimum required version
# -------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.19)

# -------------------------------------------------------------------------------------------------
# Disable in-source builds to prevent source tree corruption.
# -------------------------------------------------------------------------------------------------
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed. You should create a separate directory for build files.")
endif()

# -------------------------------------------------------------------------------------------------
# Include helper functions
# -------------------------------------------------------------------------------------------------
include(cmake/Helpers.cmake)

# -------------------------------------------------------------------------------------------------
# Get version from header file
# -------------------------------------------------------------------------------------------------

get_project_version(PROJECT_VERSION PROJECT_VERSION_MAJOR PROJECT_VERSION_MINOR PROJECT_VERSION_PATCH)
message(STATUS "FileHauler Version: ${PROJECT_VERSION}")

# -------------------------------------------------------------------------------------------------
# CMake global settings
# -------------------------------------------------------------------------------------------------

# Set the project name and version
project(FileHauler VERSION ${PROJECT_VERSION})

# Define a full uppercase version of the project name
string(TOUPPER ${CMAKE_PROJECT_NAME} CMAKE_PROJECT_NAME_UPPERCASE)

# Define a full lowercase version of the project name
string(TOLOWER ${CMAKE_PROJECT_NAME} CMAKE_PROJECT_NAME_LOWERCASE)

# Define the company namespace
set(COMPANY_NAMESPACE "FriendlyShade")
string(TOLOWER ${COMPANY_NAMESPACE} COMPANY_NAMESPACE_LOWERCASE)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)

# Force it to C++17
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Disable language extensions that are not part of the C++ standard
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable solution folders (filters that group multiple projects in Visual Studio)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

# -------------------------------------------------------------------------------------------------
# Prefixes
# -------------------------------------------------------------------------------------------------

# Set install bin folder path
set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)

# -------------------------------------------------------------------------------------------------
# Add modules and software
# -------------------------------------------------------------------------------------------------

# Add MyExecutable
add_subdirectory(software/file_hauler)