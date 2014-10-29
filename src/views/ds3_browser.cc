/*
 * *****************************************************************************
 *   Copyright 2014 Spectra Logic Corporation. All Rights Reserved.
 *   Licensed under the Apache License, Version 2.0 (the "License"). You may not
 *   use this file except in compliance with the License. A copy of the License
 *   is located at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   or in the "license" file accompanying this file.
 *   This file is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 *   CONDITIONS OF ANY KIND, either express or implied. See the License for the
 *   specific language governing permissions and limitations under the License.
 * *****************************************************************************
 */

#include <QMenu>

#include "lib/client.h"
#include "models/ds3_browser_model.h"
#include "models/session.h"
#include "views/buckets/new_bucket_dialog.h"
#include "views/ds3_browser.h"

DS3Browser::DS3Browser(Session* session, QWidget* parent, Qt::WindowFlags flags)
	: Browser(parent, flags)
{
	AddCustomToolBarActions();

	m_client = new Client(session->GetHost(),
			      session->GetPort(),
			      session->GetAccessId(),
			      session->GetSecretKey());
	m_model = new DS3BrowserModel(m_client, this);
	m_treeView->setModel(m_model);
}

DS3Browser::~DS3Browser()
{
	delete m_client;
}

void
DS3Browser::AddCustomToolBarActions()
{
	m_refreshAction = new QAction(style()->standardIcon(QStyle::SP_BrowserReload),
				      "Refresh", this);
	connect(m_refreshAction, SIGNAL(triggered()), this, SLOT(Refresh()));
	m_toolBar->addAction(m_refreshAction);
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
	Refresh();
}

void
DS3Browser::OnModelItemDoubleClick(const QModelIndex& index)
{
	if (m_model->IsBucketOrFolder(index)) {
		QString path = m_model->GetPath(index);
		m_treeView->setRootIndex(index);
		UpdatePathLabel(path);
	}
}

void
DS3Browser::Refresh()
{
	m_model->Refresh();
	UpdatePathLabel("/");
}
