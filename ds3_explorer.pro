TARGET = SpectraLogicDS3Explorer
VERSION = 0.0.0

QT += core gui widgets

CONFIG -= release
CONFIG += debug_and_release warn_on

CONFIG(debug, debug|release) {
	DESTDIR = debug
} else {
	DESTDIR = release
}

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

INCLUDEPATH += src

HEADERS = \
	src/main_window.h \
	src/lib/client.h \
	src/models/bucket.h \
	src/models/session.h \
	src/views/browser.h \
	src/views/ds3_browser.h \
	src/views/host_browser.h \
	src/views/session_dialog.h \
	src/views/session_view.h

SOURCES = \
	src/main.cc \
	src/main_window.cc \
	src/lib/client.cc \
	src/models/bucket.cc \
	src/models/session.cc \
	src/views/browser.cc \
	src/views/ds3_browser.cc \
	src/views/host_browser.cc \
	src/views/session_dialog.cc \
	src/views/session_view.cc

RESOURCES = ds3_explorer.qrc

macx {
	# Qt should normally find its default Info.plist.app file, however, a
	# bug appears to make it unable to find it when running qmake from a
	# build directory.  Thus, the default Info.plist.app file has been
	# copied to this provject and is specified.  We might want to have our
	# own custom Info.plist file anyway.
	QMAKE_INFO_PLIST=src/mac/Info.plist.app
}

msvc {
	LIBS += ds3.lib
	QMAKE_CXXFLAGS += /WX
} else {
	# Necessary on OSX at least
	exists(/usr/local/include) {
		INCLUDEPATH += /usr/local/include
	}
	# Necessary on OSX at least
	exists(/usr/local/lib) {
		LIBS += -L/usr/local/lib
	}

	LIBS += -lds3 -lcurl
}

gcc: QMAKE_CXXFLAGS += -Werror
