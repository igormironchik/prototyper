
project( Prototyper )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )

find_package( Qt6Core REQUIRED )
find_package( Qt6Widgets REQUIRED )
find_package( Qt6Gui REQUIRED )

set( SRC main.cpp )

set( CMAKE_CXX_STANDARD 14 )

set( CMAKE_CXX_STANDARD_REQUIRED ON )

qt6_add_resources( SRC Prototyper.qrc )

if( WIN32 )
	list( APPEND SRC Prototyper.rc )
endif()

add_executable( Prototyper ${SRC} )

target_link_libraries( Prototyper Protoyper.Core Qt6::Widgets Qt6::Gui Qt6::Core )

add_dependencies( Prototyper Prototyper.Core )
