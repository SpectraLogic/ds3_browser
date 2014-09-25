#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

#include "session.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(Session*);

private:
	Session* m_session;
};

#endif
