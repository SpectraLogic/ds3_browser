################################################################################
#  Copyright 2014 Spectra Logic Corporation. All Rights Reserved.
#  Licensed under the Apache License, Version 2.0 (the "License"). You may not
#  use this file except in compliance with the License. A copy of the License
#  is located at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#  or in the "license" file accompanying this file.
#  This file is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
#  CONDITIONS OF ANY KIND, either express or implied. See the License for the
#  specific language governing permissions and limitations under the License.
################################################################################

TARGET = SpectraLogicDS3Explorer
VERSION = 0.0.0

QT += concurrent core gui widgets

CONFIG -= release
CONFIG += debug_and_release warn_on

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.qrc
Debug:UI_DIR = debug/.ui

Release:DESTDIR = release
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.qrc
Release:UI_DIR = release/.ui
Release:DEFINES += NO_DEBUG

INCLUDEPATH += src

HEADERS = \
	src/main_window.h \
	src/helpers/number_helper.h \
	src/lib/work_items/bulk_work_item.h \
	src/lib/work_items/bulk_put_work_item.h \
	src/lib/work_items/object_work_item.h \
	src/lib/work_items/work_item.h \
	src/lib/client.h \
	src/lib/logger.h \
	src/lib/watchers/get_bucket_watcher.h \
	src/lib/watchers/get_service_watcher.h \
	src/models/ds3_browser_model.h \
	src/models/host_browser_model.h \
	src/models/job.h \
	src/models/session.h \
	src/views/browser.h \
	src/views/browser_tree_view_style.h \
	src/views/buckets/new_bucket_dialog.h \
	src/views/console.h \
	src/views/ds3_browser.h \
	src/views/host_browser.h \
	src/views/job_view.h \
	src/views/jobs_view.h \
	src/views/session_dialog.h \
	src/views/session_view.h

SOURCES = \
	src/main.cc \
	src/main_window.cc \
	src/helpers/number_helper.cc \
	src/lib/work_items/bulk_work_item.cc \
	src/lib/work_items/bulk_put_work_item.cc \
	src/lib/work_items/object_work_item.cc \
	src/lib/work_items/work_item.cc \
	src/lib/client.cc \
	src/lib/watchers/get_bucket_watcher.cc \
	src/lib/watchers/get_service_watcher.cc \
	src/models/ds3_browser_model.cc \
	src/models/host_browser_model.cc \
	src/models/job.cc \
	src/models/session.cc \
	src/views/browser.cc \
	src/views/browser_tree_view_style.cc \
	src/views/buckets/new_bucket_dialog.cc \
	src/views/console.cc \
	src/views/ds3_browser.cc \
	src/views/host_browser.cc \
	src/views/jobs_view.cc \
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
	QMAKE_CXXFLAGS += /WX /D_CRT_SECURE_NO_WARNINGS
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
