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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QTabWidget>

class Session;
class SessionView;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);

	inline bool IsFinished();

private:
	Session* CreateSession();
	void CreateMenus();

	bool m_isFinished;

	QMenu* m_helpMenu;
	QAction* m_aboutAction;

	QTabWidget* m_sessionTabs;
	SessionView* m_sessionView;

private slots:
	void About();
};

inline bool
MainWindow::IsFinished()
{
	return m_isFinished;
}

#endif
