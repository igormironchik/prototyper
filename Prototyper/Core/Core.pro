
TEMPLATE = lib
CONFIG += shared
TARGET = Prototyper.Core
QT += core gui widgets
VERSION = 1.0.0

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

generate_cfg.commands = ../../3rdparty/QtConfFile/qtconffile.generator -i ${QMAKE_FILE_IN} \
-o $${OUT_PWD}/${QMAKE_FILE_BASE}.hpp

PRE_TARGETDEPS += compiler_generate_cfg_make_all

HEADERS +=	form_actions.hpp \
			form.hpp \
			form_hierarchy_widget.hpp \
			form_scene.hpp \
			form_view.hpp \
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
			$$GENERATED

SOURCES +=	form_actions.cpp \
			form.cpp \
			form_hierarchy_widget.cpp \
			form_scene.cpp \
			form_view.cpp \
			grid_step_dlg.cpp \
			name_dlg.cpp \
			project_description_tab.cpp \
			project_widget.cpp \
			project_widget_tab_bar.cpp \
			project_window.cpp \
			tabs_list.cpp \
			text_editor.cpp \
			text_opts_bar.cpp \
			top_gui.cpp

FORMS +=	grid_step_dlg.ui \
			name_dlg.ui

unix|win32: LIBS += -L$$OUT_PWD/../../3rdparty/QtConfFile/lib/ -lQtConfFile

INCLUDEPATH += $$PWD/../../3rdparty/QtConfFile
DEPENDPATH += $$PWD/../../3rdparty/QtConfFile

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../3rdparty/QtConfFile/lib/QtConfFile.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../3rdparty/QtConfFile/lib/libQtConfFile.a
