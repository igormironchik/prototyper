
defineTest(minQtVersion) {
    maj = $$1
    min = $$2
    patch = $$3
    isEqual(QT_MAJOR_VERSION, $$maj) {
        isEqual(QT_MINOR_VERSION, $$min) {
            isEqual(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $$min) {
            return(true)
        }
    }
    greaterThan(QT_MAJOR_VERSION, $$maj) {
        return(true)
    }
    return(false)
}

!minQtVersion(5, 14, 0) {
	message("Cannot build Prototyper with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.14.0.")
}

CONFIG += c++11

TEMPLATE = subdirs

SUBDIRS = Prototyper \
		  3rdparty

Prototyper.depends = 3rdparty

OTHER_FILES = LICENSE README.md
