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
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QThreadPool>
#include <QWindow>

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

	CreateMenus();

	ReadSettings();
	LOG_INFO("Starting DS3 Browser Session");
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
	SessionView* sessionView = new SessionView(session, m_jobsView);
	m_sessionViews << sessionView;
	m_sessionTabs->addTab(sessionView, session->GetHost());
	// The MainWindow will not have been shown for the first time when
	// creating the initial session.
	show();
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
	LOG_INFO("Closing DS3 Browser Session");

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
	m_aboutAction = new QAction(tr("&About %1").arg(APP_NAME), this);
	connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(About()));

	m_logFileAction = new QAction(tr("&Log File Location"), this);
	connect(m_logFileAction, SIGNAL(triggered()), this, SLOT(LogFile()));

	m_viewMenu = new QMenu(tr("&View"), this);
	m_viewMenu->addAction(m_consoleDock->toggleViewAction());
	m_viewMenu->addAction(m_jobsDock->toggleViewAction());

	menuBar()->addMenu(m_viewMenu);

	m_helpMenu = new QMenu(tr("&Preferences"), this);
	m_helpMenu->addAction(m_logFileAction);

	menuBar()->addMenu(m_helpMenu);

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

void
MainWindow::LogFile()
{
	QSettings settings;
	bool loggingEnabled = settings.value("mainWindow/loggingEnabled", true).toBool();
	QString path = settings.value("mainWindow/logFilePath", QDir::homePath()).toString();
	QString file = settings.value("mainWindow/logFileName", "ds3_browser_log.txt").toString();

	m_preferences = new QWidget;
	QBoxLayout* layout = new QVBoxLayout(m_preferences);
	layout->setDirection(QBoxLayout::TopToBottom);
	QCheckBox* enableLoggingBox = new QCheckBox;
	QLineEdit* fileInputDialog = new QLineEdit;
	QDialogButtonBox* buttons = new QDialogButtonBox;

	enableLoggingBox->setCheckState(Qt::Unchecked);
	if(loggingEnabled)
		enableLoggingBox->setCheckState(Qt::Checked);
	enableLoggingBox->setText("Enable Logging to Log File");

	fileInputDialog->insert(path+file);

	QPushButton* apply = new QPushButton;
	apply->setText("Apply");
	QPushButton* browse = new QPushButton;
	browse->setText("Browse");
	buttons->addButton("Cancel", QDialogButtonBox::RejectRole);
	connect(buttons, SIGNAL(rejected()), this, SLOT(ClosePreferences()));
	buttons->addButton(apply, QDialogButtonBox::ApplyRole);
	connect(apply, SIGNAL(clicked()), this, SLOT(ApplyChanges()));
	buttons->addButton("Browse", QDialogButtonBox::ActionRole);
	connect(browse, SIGNAL(clicked()), this, SLOT(ChooseLogFile()));

	layout->setContentsMargins (5, 5, 5, 5);
	layout->setSpacing(5);
	layout->addWidget(enableLoggingBox);
	layout->addWidget(fileInputDialog);
	layout->addWidget(buttons, 1, Qt::AlignRight);
	m_preferences->setLayout(layout);
	m_preferences->show();
}

void
MainWindow::ChooseLogFile()
{
	QSettings settings;
	QString path = settings.value("mainWindow/logFilePath", QDir::homePath()).toString();
	QString file = settings.value("mainWindow/logFileName", "ds3_browser_log.txt").toString();

	m_logFileBrowser = new QWidget;
	QBoxLayout* layout = new QVBoxLayout(m_logFileBrowser);
	QFileDialog* fileDialog = new QFileDialog;

	QString defaultFilter("Text files (*.txt)");

	fileDialog->setWindowTitle("Log File Location");
	fileDialog->setDirectory(path);
	fileDialog->selectNameFilter(defaultFilter);
	fileDialog->selectFile(file);
	fileDialog->exec();

	layout->addWidget(fileDialog);
	m_logFileBrowser->setLayout(layout);
	m_logFileBrowser->show();
}

void
MainWindow::ClosePreferences()
{
	delete m_preferences;
}

void
MainWindow::ApplyChanges()
{

}