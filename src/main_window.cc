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
#include <QCloseEvent>
#include <QDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QThreadPool>

#include "lib/logger.h"

#include "global.h"
#include "main_window.h"
#include "models/session.h"
#include "views/console.h"
#include "views/jobs_view.h"
#include "views/session_dialog.h"
#include "views/session_view.h"

const int MainWindow::CANCEL_JOBS_TIMEOUT_IN_MS = 30000;

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags),
	  m_sessionTabs(new QTabWidget(this)),
	  m_jobsView(new JobsView(this))
{
	setWindowTitle(SL_APP_NAME);

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

int
MainWindow::GetNumActiveJobs() const
{
	int num = 0;
	for (int i = 0; i < m_sessionViews.size(); i++) {
		num += m_sessionViews[i]->GetNumActiveJobs();
	}
	return num;
}

void
MainWindow::CreateSession()
{
	SessionDialog* sessionDialog = static_cast<SessionDialog*>(sender());
	Session* session = new Session(sessionDialog->GetSession());
	delete sessionDialog;
	SessionView* sessionView = new SessionView(session, m_jobsView);
	m_sessionViews << sessionView;
	m_sessionTabs->addTab(sessionView, session->GetHost());
	// The MainWindow will not have been shown for the first time when
	// creating the initial session.
	show();
}

void
MainWindow::DeleteSession()
{
	SessionDialog* sessionDialog = static_cast<SessionDialog*>(sender());
	delete sessionDialog;
}


void
MainWindow::closeEvent(QCloseEvent* event)
{
	if (GetNumActiveJobs() > 0) {
		QString title = "Active Jobs In Progress";
		QString msg = "There are active jobs still in progress.  " \
			      "Are you sure wish to cancel those jobs and " \
			      "quit the applcation?";
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(this, title, msg,
					   QMessageBox::Ok |
					   QMessageBox::Cancel,
					   QMessageBox::Cancel);
		if (ret == QMessageBox::Ok) {
			CancelActiveJobs();
		} else {
			event->ignore();
			return;
		}
	}

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
MainWindow::CancelActiveJobs()
{
	for (int i = 0; i < m_sessionViews.size(); i++) {
		m_sessionViews[i]->CancelActiveJobs();
	}
	// All jobs are currently run via QtConcurrent::run, which uses
	// the global thread pool.  This will need to be modified if certain
	// job tasks are ever switched to using a custom thread pool.
	bool ret = QThreadPool::globalInstance()->waitForDone(CANCEL_JOBS_TIMEOUT_IN_MS);
	if (!ret) {
		LOG_ERROR("Timed out waiting for all jobs to stop");
	}
}

void
MainWindow::About()
{
	QString text = tr("<b>%1</b><br/>Version %2")
				.arg(APP_NAME).arg(APP_VERSION);
	QMessageBox::about(this, tr("About %1").arg(APP_NAME), text);
}
