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

include(../common.pri)

TARGET = test

QT += testlib
QT -= gui
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += ../src

# Test Code

HEADERS = \
	test.h \
	helpers/number_helper_test.h \
	lib/mime_data_test.h \
	models/ds3_url_test.h

SOURCES = \
	main.cc \
	test.cc \
	helpers/number_helper_test.cc \
	lib/mime_data_test.cc \
	models/ds3_url_test.cc

# Code that is being tested.
# TODO share this with the main .pro file

HEADERS += \
	../src/helpers/number_helper.h \
	../src/lib/mime_data.h \
	../src/models/ds3_url.h

SOURCES += \
	../src/helpers/number_helper.cc \
	../src/lib/mime_data.cc \
	../src/models/ds3_url.cc
