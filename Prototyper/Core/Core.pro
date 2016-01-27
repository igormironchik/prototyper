
TEMPLATE = lib
CONFIG += shared
TARGET = Prototyper.Core
QT += core gui widgets
VERSION = 1.0.0

DESTDIR = ../..

HEADERS += project_window.hpp \
           props_window.hpp \
           tools_window.hpp
           
SOURCES += project_window.cpp \
           props_window.cpp \
           tools_window.cpp
           
