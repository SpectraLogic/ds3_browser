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
