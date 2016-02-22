
TEMPLATE = app
TARGET = Prototyper.App
DESTDIR = ../..
QT += core gui widgets
VERSION = 1.0.0

SOURCES = main.cpp

RESOURCES = Prototyper.qrc

win32 {
    RC_FILE = Prototyper.rc
}

QMAKE_RPATHDIR += ${ORIGIN}

unix|win32: LIBS += -L$$OUT_PWD/../../ -lPrototyper.Core

INCLUDEPATH += $$PWD/../..
DEPENDPATH += $$PWD/../..
