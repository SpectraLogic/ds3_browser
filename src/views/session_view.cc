#include "models/bucket.h"
#include "views/session_view.h"

SessionView::SessionView(Session* session, QWidget* parent)
	: QWidget(parent),
	  m_session(session)
{
	m_hostFileSystem = new QFileSystemModel(this);
	m_hostFileSystem->setRootPath(QDir::rootPath());

	m_hostBrowser = new QTreeView;
	m_hostBrowser->setModel(m_hostFileSystem);
	m_hostBrowser->setRootIndex(m_hostFileSystem->index(QDir::rootPath()));

	m_remoteBrowser = new QTreeView;

	m_splitter = new QSplitter;
	m_splitter->addWidget(m_hostBrowser);
	m_splitter->addWidget(m_remoteBrowser);

	m_topLayout = new QVBoxLayout(this);
	m_topLayout->addWidget(m_splitter);

	setLayout(m_topLayout);

	m_client = new Client(m_session->GetHost(),
			      m_session->GetPort(),
			      m_session->GetAccessId(),
			      m_session->GetSecretKey());

	ds3_get_service_response* response = m_client->GetService();
	Bucket* bucket = new Bucket(response);
	m_remoteBrowser->setModel(bucket);
}

SessionView::~SessionView()
{
	delete m_client;
}
