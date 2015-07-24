/*
 * *****************************************************************************
 *   Copyright 2014-2015 Spectra Logic Corporation. All Rights Reserved.
 *   Licensed under the Apache License, Version 2.0 (the "License"). You may not
 *   use this file except in compliance with the License. A copy of the License
 *   is located at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   or in the "license" file accompanying this file.
 *   This file is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 *   CONDITIONS OF ANY KIND, either express or implied. See the License for the
 *   specific language governing permissions and limitations under the License.
 * *****************************************************************************
 */

#include <QApplication>
#include <QFile>
#include <ds3.h>
#include <QFontDatabase>

#include "global.h"
#include "models/job.h"
#include "views/console.h"

#include "main_window.h"
#include "views/session_dialog.h"

int
main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Spectra Logic");
	app.setOrganizationDomain("spectralogic.com");
	app.setApplicationName(APP_NAME);

	QFile res(":/resources/fonts/fontawesome-webfont.ttf");
	res.open(QIODevice::ReadOnly);
	QFontDatabase::addApplicationFontFromData(res.readAll());
	// Using font-awesome characters example:
	//   widget->setFont(QFont("FontAwesome"));
	//   QString searchIcon = QString::fromUtf8("\uf002");
	//   widget->setText(searchIcon);
	//
	// "\uf002" is the unicode for the search icon, their other icon codes
	//   can be found on "http://fortawesome.github.io/Font-Awesome/cheatsheet/"

	QFile qssf(":/resources/stylesheets/main.qss");
	qssf.open(QFile::ReadOnly);
	QString qss(qssf.readAll());
	qssf.close();
	app.setStyleSheet(qss);

	// Job is used as an argument in a signal/slot connection
	qRegisterMetaType<Job>();

	// Ensure the console instance is created in the main GUI thread
	Console::Instance();

	MainWindow mainWindow;
	SessionDialog sessionDialog;
	sessionDialog.show();
	QObject::connect(&sessionDialog, SIGNAL(accepted()),
			 &mainWindow, SLOT(CreateSession()));

	int ret = app.exec();

	ds3_cleanup();

	return ret;
}
