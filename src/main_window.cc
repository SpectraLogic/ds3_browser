#include <QMenuBar>
#include <QMessageBox>

#include "main_window.h"
#include "session_dialog.h"
#include "models/bucket.h"

MainWindow::MainWindow(Session* session)
	: m_session(session)
{
	setWindowTitle("Spectra Logic DS3 Explorer");

	CreateMenus();

	m_hostFileSystem = new QFileSystemModel(this);
	m_hostFileSystem->setRootPath(QDir::rootPath());

	m_hostBrowser = new QTreeView;
	m_hostBrowser->setModel(m_hostFileSystem);
	m_hostBrowser->setRootIndex(m_hostFileSystem->index(QDir::rootPath()));

	m_remoteBrowser = new QTreeView;

	m_splitter = new QSplitter;
	m_splitter->addWidget(m_hostBrowser);
	m_splitter->addWidget(m_remoteBrowser);

	setCentralWidget(m_splitter);

	m_client = new Client(m_session->GetHost(),
			      m_session->GetPort(),
			      m_session->GetAccessId(),
			      m_session->GetSecretKey());

	ds3_get_service_response* response = m_client->GetService();
	Bucket* bucket = new Bucket(response);
	m_remoteBrowser->setModel(bucket);
}

MainWindow::~MainWindow()
{
	delete m_client;
}

void
MainWindow::CreateMenus()
{
	m_aboutAction = new QAction(tr("&About"), this);
	connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(About()));

	m_helpMenu = new QMenu(tr("&Help"), this);
	m_helpMenu->addAction(m_aboutAction);

	menuBar()->addMenu(m_helpMenu);
}

void
MainWindow::About()
{
	QMessageBox::about(this, tr("About DS3 Explorer"),
		tr("<b>DS3 Explorer</b>"));
}
