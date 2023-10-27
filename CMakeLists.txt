cmake_minimum_required(VERSION 3.24)

project(EZ_SMALLVEC
VERSION 0.2.1
LANGUAGES C CXX)

set(PACKAGE_NAME ez-smallvec)
set(TARGET_NAME smallvec)

include(GNUInstallDirs)
include(FetchContent)

set(EZ_SMALLVEC_CONFIG_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PACKAGE_NAME}" CACHE STRING "The relative directory to install package config files.")

FetchContent_Declare(
	ez-cmake
	GIT_REPOSITORY "https://github.com/errata-c/ez-cmake.git"
	GIT_TAG "main"
)
FetchContent_MakeAvailable(ez-cmake)


# Create main target.
add_library(${TARGET_NAME} INTERFACE)

# Add the include directory
target_include_directories(${TARGET_NAME} INTERFACE 

	# Build only include directories
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"

	# Install include directories
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# Select the c++ version to use.
target_compile_features(${TARGET_NAME} INTERFACE cxx_std_17)

# Just for internal compatibility, so that subprojects can use the namespaced version
add_library("ez::${TARGET_NAME}" ALIAS "${TARGET_NAME}")

if(PROJECT_IS_TOP_LEVEL)
	include(CTest)

	# Build the tests once the option is enabled.
	if(BUILD_TESTING)
		add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")
	endif()

	install(
		DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
		TYPE INCLUDE
		FILES_MATCHING
		PATTERN "*.h" PATTERN "*.hpp"
	)

	install(TARGETS ${TARGET_NAME}
		EXPORT ${TARGET_NAME}-targets
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}/$<CONFIG>"
		ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/$<CONFIG>"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/$<CONFIG>"
	)

	install_package(
		NAME "${PACKAGE_NAME}"
		EXPORT "${TARGET_NAME}-targets"
		VERSION "${CMAKE_PROJECT_VERSION}"

		NAMESPACE "ez::"

		# Version compatiblity
		COMPATIBILITY "SameMajorVersion"

		# Relative install location of the package config files.
		DESTINATION "${EZ_SMALLVEC_CONFIG_DIR}"

		# Only add this option if the library is header only, or an interface library.
		ARCH_INDEPENDENT
	)

endif()