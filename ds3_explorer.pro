################################################################################
#  Copyright 2014-2015 Spectra Logic Corporation. All Rights Reserved.
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

include(common.pri)

TARGET = "BlackPearl DS3 Browser"

CONFIG -= release
CONFIG += debug_and_release warn_on

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

SOURCES += src/main.cc

RESOURCES = ds3_explorer.qrc

macx {
	# With Qt 5.4 at least, only release builds will generate the
	# Info.plist file and copy the icon file to the app bundle.  Thus, our
	# application icon will not be available for debug builds.
	# Also, the build must be done outside of the source tree in order
	# Qt to find its template Info.plist file.
	ICON = resources/icons/ds3_explorer.icns
	QMAKE_INFO_PLIST = src/mac/Info.plist.app
}

win32 {
	# Fix a qdatetime.h: warning C4003: not enough actual parameters for
	# macro compile error on Windows.
	# See http://qt-project.org/forums/viewthread/22133
	DEFINES += NOMINMAX

	RC_FILE = ds3_explorer.rc
}
