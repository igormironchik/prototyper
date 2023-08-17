
project( Core )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )

set( CMAKE_CXX_STANDARD 14 )

set( CMAKE_CXX_STANDARD_REQUIRED ON )

find_package( Qt6Core REQUIRED )
find_package( Qt6Widgets REQUIRED )
find_package( Qt6Gui REQUIRED )
find_package( Qt6Svg REQUIRED )

file( GLOB_RECURSE HSRC "*.hpp" )
file( GLOB_RECURSE CSRC "*.cpp" )
file( GLOB_RECURSE USRC "*.ui" )

list( APPEND HSRC
    ${CMAKE_CURRENT_BINARY_DIR}/windows_cfg.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/project_cfg.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/session_cfg.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/version.hpp )

set( SRC ${HSRC} ${CSRC} ${USRC} )

qt6_add_resources( SRC resources.qrc )

add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/windows_cfg.hpp
	PRE_BUILD
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../../3rdparty/cfgfile/generator/${CMAKE_CFG_INTDIR}/cfgfile.generator${CMAKE_EXECUTABLE_SUFFIX} -i windows_cfg.qtconf -o ${CMAKE_CURRENT_BINARY_DIR}/windows_cfg.hpp
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/project_cfg.hpp
	PRE_BUILD
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../../3rdparty/cfgfile/generator/${CMAKE_CFG_INTDIR}/cfgfile.generator${CMAKE_EXECUTABLE_SUFFIX} -i project_cfg.qtconf -o ${CMAKE_CURRENT_BINARY_DIR}/project_cfg.hpp
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/session_cfg.hpp
	PRE_BUILD
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../../3rdparty/cfgfile/generator/${CMAKE_CFG_INTDIR}/cfgfile.generator${CMAKE_EXECUTABLE_SUFFIX} -i session_cfg.qtconf -o ${CMAKE_CURRENT_BINARY_DIR}/session_cfg.hpp
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set_property( SOURCE ${CMAKE_CURRENT_BINARY_DIR}/session_cfg.hpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_BINARY_DIR}/project_cfg.hpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_BINARY_DIR}/windows_cfg.hpp PROPERTY SKIP_AUTOGEN ON )

add_definitions( -DPROTOTYPER_CORE -DCFGFILE_QT_SUPPORT )

include_directories( ${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/cfgfile )

add_library( Prototyper.Core SHARED ${SRC} )

add_dependencies( Prototyper.Core cfgfile.generator )

target_link_libraries( Prototyper.Core Qt6::Svg Qt6::Widgets Qt6::Gui Qt6::Core )
