
TEMPLATE = lib
CONFIG += shared
TARGET = Prototyper.Core
QT += core gui widgets
VERSION = 1.0.0

RESOURCES = resources.qrc

DESTDIR = ../..

HEADERS += project_widget.hpp \
           project_window.hpp \
           props_widget.hpp \
           props_window.hpp \
           tools_widget.hpp \
           tools_window.hpp \
           top_gui.hpp
           
SOURCES += project_widget.cpp \
           project_window.cpp \
           props_widget.cpp \
           props_window.cpp \
           tools_widget.cpp \
           tools_window.cpp \
           top_gui.cpp

TO_GENERATE = windows_cfg.qtconf

OTHER_FILES += windows_cfg.qtconf

QMAKE_EXTRA_COMPILERS += generate_cfg
generate_cfg.name = CONF_GEN
generate_cfg.input = TO_GENERATE
generate_cfg.output = ${QMAKE_FILE_BASE}.hpp
generate_cfg.CONFIG = no_link
generate_cfg.variable_out = HEADERS

generate_cfg.commands = ../../3rdparty/QtConfFile/qtconffile.generator -i ${QMAKE_FILE_IN} \
-o $${OUT_PWD}/${QMAKE_FILE_BASE}.hpp

PRE_TARGETDEPS += compiler_generate_cfg_make_all

unix|win32: LIBS += -L$$OUT_PWD/../../3rdparty/QtConfFile/lib/ -lQtConfFile

INCLUDEPATH += $$PWD/../../3rdparty/QtConfFile
DEPENDPATH += $$PWD/../../3rdparty/QtConfFile

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../3rdparty/QtConfFile/lib/QtConfFile.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../3rdparty/QtConfFile/lib/libQtConfFile.a
