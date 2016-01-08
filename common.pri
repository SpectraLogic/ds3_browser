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

# Define settings common between the main and test applications' project
# files

VERSION = 1.2.1

QT += concurrent core gui widgets

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

INCLUDEPATH += $${PWD}/src
INCLUDEPATH += $${PWD}/vendor

HEADERS = \
	$${PWD}/src/main_window.h \
	$${PWD}/src/helpers/number_helper.h \
	$${PWD}/src/lib/work_items/bulk_work_item.h \
	$${PWD}/src/lib/work_items/bulk_get_work_item.h \
	$${PWD}/src/lib/work_items/bulk_put_work_item.h \
	$${PWD}/src/lib/work_items/object_work_item.h \
	$${PWD}/src/lib/work_items/work_item.h \
	$${PWD}/src/lib/client.h \
	$${PWD}/src/lib/logger.h \
	$${PWD}/src/lib/mime_data.h \
	$${PWD}/src/lib/errors/ds3_error.h \
	$${PWD}/src/lib/watchers/get_bucket_watcher.h \
	$${PWD}/src/lib/watchers/get_service_watcher.h \
	$${PWD}/src/lib/watchers/get_objects_watcher.h \
	$${PWD}/src/models/ds3_browser_model.h \
	$${PWD}/src/models/ds3_url.h \
	$${PWD}/src/models/host_browser_model.h \
	$${PWD}/src/models/job.h \
	$${PWD}/src/models/session.h \
	$${PWD}/src/views/browser.h \
	$${PWD}/src/views/browser_tree_view_style.h \
	$${PWD}/src/views/buckets/delete_bucket_dialog.h \
	$${PWD}/src/views/buckets/new_bucket_dialog.h \
	$${PWD}/src/views/console.h \
	$${PWD}/src/views/dialog.h \
	$${PWD}/src/views/ds3_browser.h \
	$${PWD}/src/views/ds3_delete_dialog.h \
	$${PWD}/src/views/host_browser.h \
	$${PWD}/src/views/job_view.h \
	$${PWD}/src/views/jobs_view.h \
	$${PWD}/src/views/objects/delete_objects_dialog.h \
	$${PWD}/src/views/session_dialog.h \
	$${PWD}/src/views/session_view.h \
    src/views/buckets/new_folder_dialog.h

SOURCES = \
	$${PWD}/src/main_window.cc \
	$${PWD}/src/helpers/number_helper.cc \
	$${PWD}/src/lib/client.cc \
	$${PWD}/src/lib/mime_data.cc \
	$${PWD}/src/lib/errors/ds3_error.cc \
	$${PWD}/src/lib/watchers/get_bucket_watcher.cc \
	$${PWD}/src/lib/watchers/get_service_watcher.cc \
	$${PWD}/src/lib/watchers/get_objects_watcher.cc \
	$${PWD}/src/lib/work_items/bulk_work_item.cc \
	$${PWD}/src/lib/work_items/bulk_get_work_item.cc \
	$${PWD}/src/lib/work_items/bulk_put_work_item.cc \
	$${PWD}/src/lib/work_items/object_work_item.cc \
	$${PWD}/src/lib/work_items/work_item.cc \
	$${PWD}/src/models/ds3_browser_model.cc \
	$${PWD}/src/models/ds3_url.cc \
	$${PWD}/src/models/host_browser_model.cc \
	$${PWD}/src/models/job.cc \
	$${PWD}/src/models/session.cc \
	$${PWD}/src/views/browser.cc \
	$${PWD}/src/views/browser_tree_view_style.cc \
	$${PWD}/src/views/buckets/delete_bucket_dialog.cc \
	$${PWD}/src/views/buckets/new_bucket_dialog.cc \
	$${PWD}/src/views/console.cc \
	$${PWD}/src/views/dialog.cc \
	$${PWD}/src/views/ds3_browser.cc \
	$${PWD}/src/views/ds3_delete_dialog.cc \
	$${PWD}/src/views/host_browser.cc \
	$${PWD}/src/views/jobs_view.cc \
	$${PWD}/src/views/objects/delete_objects_dialog.cc \
	$${PWD}/src/views/session_dialog.cc \
	$${PWD}/src/views/session_view.cc \
    src/views/buckets/new_folder_dialog.cc

msvc {
	LIBS += ds3.lib
	LIBS += zlib_a.lib
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

	LIBS += -lds3 -lcurl -lz
}

gcc: QMAKE_CXXFLAGS += -Werror
