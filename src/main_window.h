#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QSplitter>
#include <QTreeView>

#include "session.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(Session*);

private:
	Session* m_session;

	QSplitter* m_splitter;
	QTreeView* m_hostBrowser;
	QTreeView* m_remoteBrowser;

	QFileSystemModel* m_hostFileSystem;
};

#endif
