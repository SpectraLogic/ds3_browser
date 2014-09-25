TARGET = "Spectra Logic DS3 Explorer"

QT += core gui widgets

CONFIG += build_all debug_and_release warn_on

CONFIG(debug, debug|release) {
	DESTDIR = debug
} else {
	DESTDIR = release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

HEADERS = \
	src/main_window.h

SOURCES = \
	src/main.cc \
	src/main_window.cc

macx {
	# Qt should normally find its default Info.plist.app file, however, a
	# bug appears to make it unable to find it when running qmake from a
	# build directory.  Thus, the default Info.plist.app file has been
	# copied to this provject and is specified.  We might want to have our
	# own custom Info.plist file anyway.
	QMAKE_INFO_PLIST=src/mac/Info.plist.app
}

msvc: QMAKE_CXXFLAGS += /WX
gcc: QMAKE_CXXFLAGS += -Werror
