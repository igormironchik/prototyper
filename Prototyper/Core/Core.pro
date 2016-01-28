
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
           
