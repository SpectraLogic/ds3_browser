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
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDockWidget>
#include <QList>
#include <QMenu>
#include <QPushButton>
#include <QTabWidget>
#include <QScrollArea>

#include <math.h>

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
	QString FormatFileSize();
	double DeFormatFileSize();
	void CreateLoggingPage();

	QMenu* m_editMenu;
	QMenu* m_helpMenu;
	QAction* m_aboutAction;

	QMenu* m_viewMenu;

	QMenu* m_preferencesMenu;
	QAction* m_settingsAction;

	QTabWidget* m_sessionTabs;
	QList<SessionView*> m_sessionViews;

	JobsView* m_jobsView;
	QDockWidget* m_jobsDock;
	QScrollArea* m_jobsScroll;

	Console* m_console;
	QDockWidget* m_consoleDock;

	QTabWidget* m_tabs;
	QWidget* m_logging;
	QCheckBox* m_enableLoggingBox;
	QLineEdit* m_fileInputDialog;
	QLineEdit* m_fileSizeInput;
	QLineEdit* m_logNumberInput;
	QComboBox* m_fileSizeSuffix;
	QPushButton* m_browse;
	QWidget* m_logFileBrowser;
	QString  m_logFile;
	double m_logFileSize;
	QString m_logFileSizeSuffix;
	int m_logNumberLimit;

private slots:
	void About();
	void Settings();
	void ChooseLogFile();
	void ClosePreferences();
	void ApplyChanges();
	void ChangedEnabled(int state);
};

#endif
