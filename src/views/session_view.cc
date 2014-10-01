#include "views/session_view.h"

SessionView::SessionView(Session* session, QWidget* parent)
	: QWidget(parent),
	  m_session(session)
{
	m_hostBrowser = new HostBrowser;
	m_ds3Browser = new DS3Browser(m_session);

	m_splitter = new QSplitter;
	m_splitter->addWidget(m_hostBrowser);
	m_splitter->addWidget(m_ds3Browser);
	m_splitter->setCollapsible(0, false);
	m_splitter->setCollapsible(1, false);

	m_topLayout = new QVBoxLayout(this);
	m_topLayout->setContentsMargins(5, 5, 5, 5);
	m_topLayout->addWidget(m_splitter);

	setLayout(m_topLayout);
}

SessionView::~SessionView()
{
	delete m_session;
}
