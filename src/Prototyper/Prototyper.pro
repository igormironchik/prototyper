
TEMPLATE = app
TARGET = Prototyper
DESTDIR = ../..
QT += core gui widgets
VERSION = 1.0.0

SOURCES = main.cpp

RESOURCES = Prototyper.qrc

win32 {
    RC_FILE = Prototyper.rc
}

macx {
	QMAKE_LFLAGS += -Wl,-rpath,@loader_path/../,-rpath,@executable_path/../
} else:linux-* {
	QMAKE_RPATHDIR += \$\$ORIGIN
	QMAKE_RPATHDIR += \$\$ORIGIN/../lib
	RPATH = $$join( QMAKE_RPATHDIR, ":" )

	QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${RPATH}\'
	QMAKE_RPATHDIR =
}

unix|win32: LIBS += -L$$OUT_PWD/../../ -lPrototyper.Core

INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..
