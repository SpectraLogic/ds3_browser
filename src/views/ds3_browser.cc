#include "views/ds3_browser.h"

DS3Browser::DS3Browser(Session* session, QWidget* parent, Qt::WindowFlags flags)
	: Browser(parent, flags)
{
	m_client = new Client(session->GetHost(),
			      session->GetPort(),
			      session->GetAccessId(),
			      session->GetSecretKey());
	ds3_get_service_response* response = m_client->GetService();
	m_model = new Bucket(response, this);
	m_treeView->setModel(m_model);
}

DS3Browser::~DS3Browser()
{
	delete m_client;
}
