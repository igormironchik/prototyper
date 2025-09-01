
cmake_minimum_required( VERSION 3.19 )

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "Release"
		CACHE STRING "Choose the type of build."
		FORCE)
endif( NOT CMAKE_BUILD_TYPE )

SET( CMAKE_CXX_STANDARD 14 )

SET( CMAKE_CXX_STANDARD_REQUIRED ON )

project( src )

add_subdirectory( Core )
add_subdirectory( Prototyper )

execute_process( COMMAND git rev-parse --short HEAD
	OUTPUT_VARIABLE PROTOTYPER_VERSION_MINOR
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	OUTPUT_STRIP_TRAILING_WHITESPACE )

set( PROTOTYPER_VERSION "1.0.0 build ${PROTOTYPER_VERSION_MINOR}" )

configure_file( Core/version.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/Core/version.hpp )
