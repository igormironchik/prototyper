
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

set( SRC exporter.hpp
    export.hpp
    form_actions.hpp
    form_aspect_ratio_handle.hpp
    form_button.hpp
    form_checkbox.hpp
    form_combobox.hpp
    page.hpp
    form_grid_snap.hpp
    form_group.hpp
    form_hslider.hpp
    form_image_handles.hpp
    form_image.hpp
    form_line.hpp
    form_move_handle.hpp
    form_object.hpp
    form_polyline.hpp
    form_radio_button.hpp
    form_rectangle.hpp
    form_rect_placer.hpp
    form_resizable.hpp
    form_resize_handle.hpp
    page_scene.hpp
    form_spinbox.hpp
    form_text.hpp
    form_text_opts.hpp
    page_view.hpp
    form_vslider.hpp
    form_with_resize_and_move_handles.hpp
    grid_step_dlg.hpp
    html_exporter.hpp
    name_dlg.hpp
    pdf_exporter.hpp
    project_description_tab.hpp
    project_widget.hpp
    project_widget_tab_bar.hpp
    project_window.hpp
    tabs_list.hpp
    text_editor.hpp
    text_opts_bar.hpp
    top_gui.hpp
    utils.hpp
    form_checkbox_properties.hpp
    form_spinbox_properties.hpp
    svg_exporter.hpp
    form_undo_commands.hpp
    constants.hpp
    form_object_properties.hpp
    form_checkbox_private.hpp
    exporter_private.hpp
    form_move_handle_private.hpp
    form_resizable_private.hpp
    page_private.hpp
    types.hpp
    comments.hpp
    form_comment.hpp
    link_dlg.hpp
    form_text_properties.hpp
    form_text_style_properties.hpp
    exporter.cpp
    form_actions.cpp
    form_aspect_ratio_handle.cpp
    form_button.cpp
    form_checkbox.cpp
    form_combobox.cpp
    page.cpp
    form_grid_snap.cpp
    form_group.cpp
    form_hslider.cpp
    form_image_handles.cpp
    form_image.cpp
    form_line.cpp
    form_move_handle.cpp
    form_object.cpp
    form_polyline.cpp
    form_radio_button.cpp
    form_rectangle.cpp
    form_rect_placer.cpp
    form_resizable.cpp
    form_resize_handle.cpp
    page_scene.cpp
    form_spinbox.cpp
    form_text.cpp
    form_text_opts.cpp
    page_view.cpp
    form_vslider.cpp
    form_with_resize_and_move_handles.cpp
    grid_step_dlg.cpp
    html_exporter.cpp
    name_dlg.cpp
    pdf_exporter.cpp
    project_description_tab.cpp
    project_widget.cpp
    project_widget_tab_bar.cpp
    project_window.cpp
    tabs_list.cpp
    text_editor.cpp
    text_opts_bar.cpp
    top_gui.cpp
    utils.cpp
    form_checkbox_properties.cpp
    form_spinbox_properties.cpp
    svg_exporter.cpp
    form_undo_commands.cpp
    form_object_properties.cpp
    comments.cpp
    form_comment.cpp
    link_dlg.cpp
    form_text_properties.cpp
    form_text_style_properties.cpp
	grid_step_dlg.ui
    name_dlg.ui
    form_checkbox_properties.ui
    form_spinbox_properties.ui
    form_object_properties.ui
    comments.ui
    link.ui
    form_text_properties.ui
    form_text_style_properties.ui
    ${CMAKE_CURRENT_BINARY_DIR}/windows_cfg.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/project_cfg.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/session_cfg.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/version.hpp )

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

add_definitions( -DPROTOTYPER_CORE -DCFGFILE_QT_SUPPORT )

add_library( Prototyper.Core SHARED ${SRC} )

add_dependencies( Prototyper.Core cfgfile.generator )

target_link_libraries( Prototyper.Core Qt6::Svg Qt6::Widgets Qt6::Gui Qt6::Core )
