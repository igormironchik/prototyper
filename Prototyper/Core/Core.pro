
TEMPLATE = lib
CONFIG += shared
TARGET = Prototyper.Core
QT += core gui widgets
CONFIG += c++14

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

generate_cfg.commands = $${OUT_PWD}/../../3rdparty/QtConfFile/qtconffile.generator -i ${QMAKE_FILE_IN} \
-o $${OUT_PWD}/${QMAKE_FILE_BASE}.hpp

PRE_TARGETDEPS += compiler_generate_cfg_make_all

HEADERS +=	form_actions.hpp \
			form.hpp \
			form_group.hpp \
			form_hierarchy_widget.hpp \
			form_image.hpp \
			form_line.hpp \
			form_move_handle.hpp \
			form_object.hpp \
			form_polyline.hpp \
			form_rect_placer.hpp \
			form_resizable.hpp \
			form_resize_handle.hpp \
			form_scene.hpp \
			form_text.hpp \
			form_view.hpp \
			grid_snap.hpp \
			grid_step_dlg.hpp \
			name_dlg.hpp \
			project_description_tab.hpp \
			project_widget.hpp \
			project_widget_tab_bar.hpp \
			project_window.hpp \
			tabs_list.hpp \
			text_editor.hpp \
			text_opts_bar.hpp \
			top_gui.hpp \
			with_resize_and_move_handles.hpp \
			$$GENERATED

SOURCES +=	form_actions.cpp \
			form.cpp \
			form_group.cpp \
			form_hierarchy_widget.cpp \
			form_image.cpp \
			form_line.cpp \
			form_move_handle.cpp \
			form_object.cpp \
			form_polyline.cpp \
			form_rect_placer.cpp \
			form_resizable.cpp \
			form_resize_handle.cpp \
			form_scene.cpp \
			form_text.cpp \
			form_view.cpp \
			grid_snap.cpp \
			grid_step_dlg.cpp \
			name_dlg.cpp \
			project_description_tab.cpp \
			project_widget.cpp \
			project_widget_tab_bar.cpp \
			project_window.cpp \
			tabs_list.cpp \
			text_editor.cpp \
			text_opts_bar.cpp \
			top_gui.cpp \
			with_resize_and_move_handles.cpp

FORMS +=	grid_step_dlg.ui \
			name_dlg.ui

unix|win32: LIBS += -L$$OUT_PWD/../../3rdparty/QtConfFile/lib/ -lQtConfFile

INCLUDEPATH += $$PWD/../../3rdparty/QtConfFile
DEPENDPATH += $$PWD/../../3rdparty/QtConfFile

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../3rdparty/QtConfFile/lib/QtConfFile.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../3rdparty/QtConfFile/lib/libQtConfFile.a
