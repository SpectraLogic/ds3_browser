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
	SessionDialog* sessionDialog = new SessionDialog;
	sessionDialog->show();
	QObject::connect(sessionDialog, SIGNAL(accepted()),
			 &mainWindow, SLOT(CreateSession()));
	QObject::connect(sessionDialog, SIGNAL(rejected()),
			 &mainWindow, SLOT(DeleteSession()));

	return app.exec();
}
