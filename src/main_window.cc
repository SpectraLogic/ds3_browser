#include <QFileSystemModel>
#include <QTreeView>
#include <QTimer>

#include "main_window.h"
#include "session_dialog.h"

MainWindow::MainWindow(Session* session)
	: m_session(session)
{
	setWindowTitle("Spectra Logic DS3 Explorer");

	QFileSystemModel *model = new QFileSystemModel;
	model->setRootPath(QDir::rootPath());

	QTreeView *tree = new QTreeView();
	tree->setModel(model);
	tree->setRootIndex(model->index(QDir::rootPath()));

	setCentralWidget(tree);
}
