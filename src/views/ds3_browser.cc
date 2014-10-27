#include <QMenu>

#include "lib/client.h"
#include "models/bucket.h"
#include "models/session.h"
#include "views/buckets/new_bucket_dialog.h"
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

void
DS3Browser::GoToParent()
{
}

void
DS3Browser::GoToRoot()
{
}

void
DS3Browser::OnContextMenuRequested(const QPoint& /*pos*/)
{
	QMenu menu;
	QAction newBucketAction("New Bucket", &menu);
	menu.addAction(&newBucketAction);

	QAction* selectedAction = menu.exec(QCursor::pos());
	if (!selectedAction)
	{
		return;
	}

	if (selectedAction == &newBucketAction)
	{
		CreateBucket();
	}
}

void
DS3Browser::CreateBucket()
{
	NewBucketDialog newBucketDialog(m_client);
	if (newBucketDialog.exec() == QDialog::Rejected) {
		return;
	}
	// TODO refresh m_model
}

void
DS3Browser::OnModelItemDoubleClick(const QModelIndex& /*index*/)
{
	// TODO descend into bucket/folder if index points to a bucket/folder
}
