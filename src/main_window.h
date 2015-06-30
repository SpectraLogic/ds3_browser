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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QDockWidget>
#include <QList>
#include <QMenu>
#include <QTabWidget>
#include <QScrollArea>

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QPushButton>

class Console;
class JobsView;
class Session;
class SessionView;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	static const int CANCEL_JOBS_TIMEOUT_IN_MS;

	MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);

	int GetNumActiveJobs() const;

public slots:
	// Create a session from the connected SessionDialog.  This should
	// be connected to a SessionDialog's accepted signal.
	void CreateSession();

protected:
	void closeEvent(QCloseEvent* event);

private:
	void ReadSettings();
	void CreateMenus();
	void CancelActiveJobs();

	QMenu* m_helpMenu;
	QAction* m_aboutAction;

	QMenu* m_viewMenu;

	QMenu* m_preferencesMenu;
	QAction* m_logFileAction;

	QTabWidget* m_sessionTabs;
	QList<SessionView*> m_sessionViews;

	JobsView* m_jobsView;
	QDockWidget* m_jobsDock;
	QScrollArea* m_jobsScroll;

	Console* m_console;
	QDockWidget* m_consoleDock;

	QWidget* m_preferences;
	QCheckBox* m_enableLoggingBox;
	QLineEdit* m_fileInputDialog;
	QWidget* m_logFileBrowser;
	QString  m_logFile;

private slots:
	void About();
	void LogFile();
	void ChooseLogFile();
	void ClosePreferences();
	void ApplyChanges();
};

#endif
