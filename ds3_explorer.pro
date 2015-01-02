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

include(common.pri)

TARGET = SpectraLogicDS3Explorer

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
	ICON = resources/icons/ds3_explorer.icns

	# Qt should normally find its default Info.plist.app file, however, a
	# bug appears to make it unable to find it when running qmake from a
	# build directory.  Thus, the default Info.plist.app file has been
	# copied to this provject and is specified.  We might want to have our
	# own custom Info.plist file anyway.
	QMAKE_INFO_PLIST = src/mac/Info.plist.app
}

win32 {
	RC_FILE = ds3_explorer.rc
}
