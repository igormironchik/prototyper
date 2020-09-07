
TEMPLATE = subdirs

PROTOTYPER_VERSION_MINOR = $$system( "git rev-parse --short HEAD" )
PROTOTYPER_VERSION = 1.0.0.minor
PROTOTYPER_VERSION =  $$replace(PROTOTYPER_VERSION, minor, $$PROTOTYPER_VERSION_MINOR)

message($$PROTOTYPER_VERSION_MINOR)
message($$PROTOTYPER_VERSION)

SUBDIRS = Core \
          Prototyper

Prototyper.depends = Core

version.input = Core/version.hpp.in
version.output = $$OUT_PWD/Core/version.hpp

QMAKE_SUBSTITUTES += version
