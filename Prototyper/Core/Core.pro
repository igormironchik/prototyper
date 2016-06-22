
TEMPLATE = lib
CONFIG += shared
TARGET = Prototyper.Core
QT += core gui widgets svg
CONFIG += c++14
DEFINES += PROTOTYPER_CORE

RESOURCES = resources.qrc

DESTDIR = ../..

TO_GENERATE =	windows_cfg.qtconf \
				project_cfg.qtconf \
				session_cfg.qtconf

GENERATED =

QMAKE_EXTRA_COMPILERS += generate_cfg
generate_cfg.name = CONF_GEN
generate_cfg.input = TO_GENERATE
generate_cfg.output = ${QMAKE_FILE_BASE}.hpp
generate_cfg.CONFIG = no_link
generate_cfg.variable_out = GENERATED

generate_cfg.commands = $$shell_path( $$absolute_path( $${OUT_PWD}/../../3rdparty/QtConfFile/qtconffile.generator ) ) \
-i ${QMAKE_FILE_IN} \
-o $${OUT_PWD}/${QMAKE_FILE_BASE}.hpp

PRE_TARGETDEPS += compiler_generate_cfg_make_all

HEADERS +=	desc_window.hpp \
			exporter.hpp \
			export.hpp \
			form_actions.hpp \
			form_aspect_ratio_handle.hpp \
			form_button_properties.hpp \
			form_button.hpp \
			form_checkbox.hpp \
			form_combobox.hpp \
			form.hpp \
			form_grid_snap.hpp \
			form_group.hpp \
			form_hierarchy_model.hpp \
			form_hierarchy_widget.hpp \
			form_hslider.hpp \
			form_image_handles.hpp \
			form_image.hpp \
			form_line.hpp \
			form_move_handle.hpp \
			form_object.hpp \
			form_polyline.hpp \
			form_radio_button.hpp \
			form_rectangle.hpp \
			form_rect_placer.hpp \
			form_resizable.hpp \
			form_resize_handle.hpp \
			form_scene.hpp \
			form_size_dlg.hpp \
			form_spinbox.hpp \
			form_text.hpp \
			form_text_opts.hpp \
			form_view.hpp \
			form_vslider.hpp \
			form_with_resize_and_move_handles.hpp \
			grid_step_dlg.hpp \
			html_exporter.hpp \
			link_dlg.hpp \
			name_dlg.hpp \
			new_form_dlg.hpp \
			pdf_exporter.hpp \
			project_description_tab.hpp \
			project_widget.hpp \
			project_widget_tab_bar.hpp \
			project_window.hpp \
			tabs_list.hpp \
			text_editor.hpp \
			text_opts_bar.hpp \
			top_gui.hpp \
			utils.hpp \
			$$GENERATED \
    form_checkbox_properties.hpp \
    form_spinbox_properties.hpp \
    form_properties.hpp \
    svg_exporter.hpp \
    form_undo_commands.hpp

SOURCES +=	desc_window.cpp \
			exporter.cpp \
			form_actions.cpp \
			form_aspect_ratio_handle.cpp \
			form_button_properties.cpp \
			form_button.cpp \
			form_checkbox.cpp \
			form_combobox.cpp \
			form.cpp \
			form_grid_snap.cpp \
			form_group.cpp \
			form_hierarchy_model.cpp \
			form_hierarchy_widget.cpp \
			form_hslider.cpp \
			form_image_handles.cpp \
			form_image.cpp \
			form_line.cpp \
			form_move_handle.cpp \
			form_object.cpp \
			form_polyline.cpp \
			form_radio_button.cpp \
			form_rectangle.cpp \
			form_rect_placer.cpp \
			form_resizable.cpp \
			form_resize_handle.cpp \
			form_scene.cpp \
			form_size_dlg.cpp \
			form_spinbox.cpp \
			form_text.cpp \
			form_text_opts.cpp \
			form_view.cpp \
			form_vslider.cpp \
			form_with_resize_and_move_handles.cpp \
			grid_step_dlg.cpp \
			html_exporter.cpp \
			link_dlg.cpp \
			name_dlg.cpp \
			new_form_dlg.cpp \
			pdf_exporter.cpp \
			project_description_tab.cpp \
			project_widget.cpp \
			project_widget_tab_bar.cpp \
			project_window.cpp \
			tabs_list.cpp \
			text_editor.cpp \
			text_opts_bar.cpp \
			top_gui.cpp \
			utils.cpp \
    form_checkbox_properties.cpp \
    form_spinbox_properties.cpp \
    form_properties.cpp \
    svg_exporter.cpp \
    form_undo_commands.cpp

FORMS +=	form_size_dlg.ui \
			grid_step_dlg.ui \
			link_dlg.ui \
			name_dlg.ui \
			new_form_dlg.ui \
    form_button_properties.ui \
    form_checkbox_properties.ui \
    form_spinbox_properties.ui \
    form_properties.ui

unix|win32: LIBS += -L$$OUT_PWD/../../3rdparty/QtConfFile/lib/ -lQtConfFile

INCLUDEPATH += $$PWD/../../3rdparty/QtConfFile
DEPENDPATH += $$PWD/../../3rdparty/QtConfFile

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../3rdparty/QtConfFile/lib/QtConfFile.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../3rdparty/QtConfFile/lib/libQtConfFile.a
