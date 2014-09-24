#include <QFileSystemModel>
#include <QTreeView>

#include "main_window.h"

MainWindow::MainWindow()
{
	setWindowTitle("Spectra Logic DS3 Explorer");

	QFileSystemModel *model = new QFileSystemModel;
	model->setRootPath(QDir::rootPath());

	QTreeView *tree = new QTreeView();
	tree->setModel(model);
	tree->setRootIndex(model->index(QDir::rootPath()));

	setCentralWidget(tree);
}
