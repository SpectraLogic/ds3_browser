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
	LOG_INFO("STARTING     DS3       Browser Session");
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
	LOG_INFO("CLOSING      DS3       Browser Session");

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

	m_helpMenu = new QMenu(tr("&Settings"), this);
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
		LOG_ERROR("ERROR:       TIMED OUT waiting for all jobs to stop");
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
	// Default log name based on app name
	QString logName = APP_NAME;
	logName.replace(" ", "_");
	logName = logName.toCaseFolded()+".log";
	m_logFile = settings.value("mainWindow/logFileName", QDir::homePath()+"/"+logName).toString();
	bool loggingEnabled = settings.value("mainWindow/loggingEnabled", true).toBool();
	m_logFileSize = settings.value("mainWindow/logFileSize", 52428800).toULongLong();

	m_preferences = new QWidget;
	m_enableLoggingBox = new QCheckBox;
	m_fileSizeInput = new QLineEdit;
	m_fileSizeSuffix = new QComboBox;
	m_fileInputDialog = new QLineEdit;
	QDialogButtonBox* buttons = new QDialogButtonBox;
	QGridLayout* layout = new QGridLayout(m_preferences);

	m_enableLoggingBox->setCheckState(Qt::Unchecked);
	if(loggingEnabled)
		m_enableLoggingBox->setCheckState(Qt::Checked);
	m_enableLoggingBox->setText("Enable Logging to Log File");

	m_fileSizeInput->setText(FormatFileSize());

	m_fileSizeSuffix->addItem(tr("KB"));
	m_fileSizeSuffix->addItem(tr("MB"));
	m_fileSizeSuffix->addItem(tr("GB"));
	m_fileSizeSuffix->addItem(tr("TB"));
	if(m_logFileSizeSuffix == "KB") {
		m_fileSizeSuffix->setCurrentIndex(0);
	} else if(m_logFileSizeSuffix == "MB") {
		m_fileSizeSuffix->setCurrentIndex(1);
	} else if(m_logFileSizeSuffix == "GB") {
		m_fileSizeSuffix->setCurrentIndex(2);
	} else {
		m_fileSizeSuffix->setCurrentIndex(3);
	}

	m_fileInputDialog->setText(m_logFile);
	QSizePolicy sp = m_fileInputDialog->sizePolicy();
	sp.setHorizontalStretch(1);
	m_fileInputDialog->setSizePolicy(sp);

	QPushButton* ok = new QPushButton;
	ok->setText("OK");
	QPushButton* browse = new QPushButton;
	browse->setText("Browse");
	buttons->addButton("Cancel", QDialogButtonBox::RejectRole);
	buttons->addButton(ok, QDialogButtonBox::ApplyRole);
	connect(browse, SIGNAL(clicked(bool)), this, SLOT(ChooseLogFile()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(ClosePreferences()));
	connect(ok, SIGNAL(clicked(bool)), this, SLOT(ApplyChanges()));

	layout->setContentsMargins (6, 6, 6, 6);
	layout->setHorizontalSpacing(6);
	layout->setVerticalSpacing(6);
	layout->addWidget(m_enableLoggingBox, 1, 1, 1, 3, Qt::AlignLeft);
	layout->addWidget(m_fileSizeInput, 2, 1, 1, 2);
	layout->addWidget(m_fileSizeSuffix, 2, 3, 1, 1, Qt::AlignRight);
	layout->addWidget(m_fileInputDialog, 3, 1, 1, 2);
	layout->addWidget(browse, 3, 3, 1, 1, Qt::AlignRight);
	layout->addWidget(buttons, 4, 1, 1, 3, Qt::AlignRight);
	m_preferences->setLayout(layout);
	m_preferences->setFixedHeight(m_preferences->sizeHint().height());
	m_preferences->setWindowModality(Qt::WindowModal);
	m_preferences->move(0,0);
	m_preferences->show();
}

void
MainWindow::ChooseLogFile()
{
	QSettings settings;

	m_logFileBrowser = new QWidget;
	QBoxLayout* layout = new QVBoxLayout(m_logFileBrowser);
	QFileDialog* fileDialog = new QFileDialog;

	QString defaultFilter("Text files (*.txt)");

	fileDialog->setAcceptMode(QFileDialog::AcceptSave);
	fileDialog->setWindowTitle("Log File Location");
	fileDialog->selectNameFilter(defaultFilter);
	fileDialog->selectFile(m_logFile);
	if(fileDialog->exec()) {
		layout->addWidget(fileDialog);
		m_logFileBrowser->setLayout(layout);
		m_logFile = fileDialog->selectedFiles()[0];
		m_fileInputDialog->setText(m_logFile);
		delete layout;
		delete fileDialog;
		delete m_logFileBrowser;
	}
}

void
MainWindow::ClosePreferences()
{
	delete m_preferences;
}

void
MainWindow::ApplyChanges()
{
	QSettings settings;
	settings.setValue("mainWindow/loggingEnabled", m_enableLoggingBox->checkState());
	m_logFile = m_fileInputDialog->text();
	settings.setValue("mainWindow/logFileName", m_logFile);
	m_logFileSizeSuffix = m_fileSizeSuffix->currentText();
	settings.setValue("mainWindow/logFileSize", DeFormatFileSize());
	ClosePreferences();
}

QString
MainWindow::FormatFileSize()
{
	double sizeHolder = m_logFileSize;
	m_logFileSizeSuffix = "KB";

	if(sizeHolder > 1024) {
		sizeHolder /= 1024.0;
		m_logFileSizeSuffix = "KB";
	}
	if(sizeHolder > 1024) {
		sizeHolder /= 1024.0;
		m_logFileSizeSuffix = "MB";
	}
	if(sizeHolder > 1024) {
		sizeHolder /= 1024.0;
		m_logFileSizeSuffix = "GB";
	}
	if(sizeHolder > 1024) {
		sizeHolder /= 1024.0;
		m_logFileSizeSuffix = "TB";
	}

	// Round to two decimal places
	double ret = ceil((sizeHolder*100)-0.49)/100.0;
	return QString::number(ret);
}

double
MainWindow::DeFormatFileSize()
{
	double sizeHolder = m_fileSizeInput->text().toDouble();
	LOG_INFO(QString::number(sizeHolder));
	if(m_logFileSizeSuffix == "TB") {
		sizeHolder *= 1024.0;
		m_logFileSizeSuffix = "GB";
	}
	if(m_logFileSizeSuffix == "GB") {
		sizeHolder *= 1024.0;
		m_logFileSizeSuffix = "MB";
	}
	if(m_logFileSizeSuffix == "MB") {
		sizeHolder *= 1024.0;
		m_logFileSizeSuffix = "KB";
	}
	if(m_logFileSizeSuffix == "KB") {
		sizeHolder *= 1024.0;
	}

	double ret = ceil((sizeHolder*100)-0.49)/100.0;
	LOG_INFO(QString::number(ret));

	return ret;
}
