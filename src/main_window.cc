#include <QDesktopWidget>
#include <QRect>
#include <QSize>

#include "main_window.h"
#include "session_dialog.h"

MainWindow::MainWindow(Session* session)
	: m_session(session)
{
	setWindowTitle("Spectra Logic DS3 Explorer");

	m_hostFileSystem = new QFileSystemModel(this);
	m_hostFileSystem->setRootPath(QDir::rootPath());

	m_hostBrowser = new QTreeView();
	m_hostBrowser->setModel(m_hostFileSystem);
	m_hostBrowser->setRootIndex(m_hostFileSystem->index(QDir::rootPath()));

	m_remoteBrowser = new QTreeView;

	m_splitter = new QSplitter(this);
	m_splitter->addWidget(m_hostBrowser);
	m_splitter->addWidget(m_remoteBrowser);

	setCentralWidget(m_splitter);

	QDesktopWidget desktop;
	QRect screenSize = desktop.availableGeometry(this);
	QSize mainWindowSize(screenSize.width() * 0.5f,
			     screenSize.height() * 0.5f);
	this->setMinimumSize(mainWindowSize);
}
