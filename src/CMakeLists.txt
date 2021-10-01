
cmake_minimum_required( VERSION 3.1 )

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
    RESULT_VARIABLE PROTOTYPER_VERSION_MINOR )

set( PROTOTYPER_VERSION "1.0.0.${PROTOTYPER_VERSION_MINOR}" )

configure_file( Core/version.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/Core/version.hpp )
