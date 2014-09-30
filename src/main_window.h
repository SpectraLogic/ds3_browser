#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>

#include "views/session_view.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(Session*);

private:
	void CreateMenus();

	QMenu* m_helpMenu;
	QAction* m_aboutAction;

	SessionView* m_sessionView;

private slots:
	void About();
};

#endif
