/*
 * *****************************************************************************
 *   Copyright 2014 Spectra Logic Corporation. All Rights Reserved.
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

#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>

#include "main_window.h"
#include "models/session.h"
#include "views/console.h"
#include "views/jobs_view.h"
#include "views/session_dialog.h"
#include "views/session_view.h"

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags),
	  m_isFinished(false),
	  m_sessionTabs(new QTabWidget(this)),
	  m_jobsView(new JobsView(this))
{
	setWindowTitle("Spectra Logic DS3 Explorer");

	Session* session = CreateSession();
	if (session == NULL)
	{
		// User closed/cancelled the New Session dialog which should
		// result in the application closing.
		m_isFinished = true;
		return;
	}

	CreateMenus();

	setCentralWidget(m_sessionTabs);

	m_jobsDock = new QDockWidget("Jobs", this);
	m_jobsDock->setObjectName("jobs dock");
	m_jobsScroll = new QScrollArea;
	m_jobsScroll->setWidget(m_jobsView);
	m_jobsScroll->setWidgetResizable(true);
	m_jobsDock->setWidget(m_jobsScroll);
	addDockWidget(Qt::BottomDockWidgetArea, m_jobsDock);

	m_consoleDock = new QDockWidget("Log", this);
	m_consoleDock->setObjectName("console dock");
	m_consoleDock->setWidget(Console::Instance());
	addDockWidget(Qt::BottomDockWidgetArea, m_consoleDock);

	tabifyDockWidget(m_jobsDock, m_consoleDock);
	setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::North);

	ReadSettings();
}

void
MainWindow::closeEvent(QCloseEvent* event)
{
	QSettings settings;
	settings.setValue("mainWindow/geometry", saveGeometry());
	settings.setValue("mainWindow/windowState", saveState());
	QMainWindow::closeEvent(event);
}

void
MainWindow::ReadSettings()
{
	QSettings settings;
	restoreGeometry(settings.value("mainWindow/geometry").toByteArray());
	restoreState(settings.value("mainWindow/windowState").toByteArray());
}

Session*
MainWindow::CreateSession()
{
	SessionDialog sessionDialog;
	if (sessionDialog.exec() == QDialog::Rejected) {
		return NULL;
	}

	Session* session = new Session(sessionDialog.GetSession());
	SessionView* sessionView = new SessionView(session, m_jobsView);
	m_sessionViews << sessionView;
	m_sessionTabs->addTab(sessionView, session->GetHost());

	return session;
}

void
MainWindow::CreateMenus()
{
	m_aboutAction = new QAction(tr("&About"), this);
	connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(About()));

	m_helpMenu = new QMenu(tr("&Help"), this);
	m_helpMenu->addAction(m_aboutAction);

	menuBar()->addMenu(m_helpMenu);
}

void
MainWindow::About()
{
	QString text = tr("<b>DS3 Explorer</b><br/>Version %1")
				.arg(APP_VERSION);
	QMessageBox::about(this, tr("About DS3 Explorer"), text);
}
