/*
 * *****************************************************************************
 *   Copyright 2014-2015 Spectra Logic Corporation. All Rights Reserved.
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
#include "lib/logger.h"
#include "models/ds3_browser_model.h"
#include "models/session.h"
#include "views/ds3_delete_dialog.h"
#include "views/buckets/new_bucket_dialog.h"
#include "views/buckets/delete_bucket_dialog.h"
#include "views/objects/delete_objects_dialog.h"
#include "views/ds3_browser.h"
#include "views/jobs_view.h"

DS3Browser::DS3Browser(Client* client, JobsView* jobsView,
		       QWidget* parent, Qt::WindowFlags flags)
	: Browser(client, parent, flags),
	  m_jobsView(jobsView)
{
	AddCustomToolBarActions();

	m_model = new DS3BrowserModel(m_client, this);
	m_model->SetView(m_treeView);
	m_treeView->setModel(m_model);

	connect(m_treeView, SIGNAL(clicked(const QModelIndex&)),
		this, SLOT(OnModelItemClick(const QModelIndex&)));

	connect(m_client, SIGNAL(JobProgressUpdate(const Job)),
		this, SLOT(HandleJobUpdate(const Job)));

	connect(m_client, SIGNAL(JobProgressUpdate(const Job)),
		m_jobsView, SLOT(UpdateJob(const Job)));
}

void
DS3Browser::HandleJobUpdate(const Job job)
{
	Job::State state = job.GetState();
	if (state == Job::FINISHED) {
		Refresh();
	}
}

void
DS3Browser::AddCustomToolBarActions()
{
	m_rootAction = new QAction(QIcon(":/resources/icons/root_directory.png"),
				   "Root directory", this);
	connect(m_rootAction, SIGNAL(triggered()), this, SLOT(GoToRoot()));
	m_toolBar->addAction(m_rootAction);

	m_refreshAction = new QAction(style()->standardIcon(QStyle::SP_BrowserReload),
				      "Refresh", this);
	connect(m_refreshAction, SIGNAL(triggered()), this, SLOT(Refresh()));
	m_toolBar->addAction(m_refreshAction);
}

QString
DS3Browser::IndexToPath(const QModelIndex& index) const
{
	return m_model->GetPath(index);
}

void
DS3Browser::OnContextMenuRequested(const QPoint& /*pos*/)
{
	QMenu menu;
	QAction newBucketAction("New Bucket", &menu);
	QAction deleteAction("Delete", &menu);

	QModelIndex index = m_treeView->rootIndex();
	bool atBucketListingLevel = !index.isValid();
	if (atBucketListingLevel) {
		menu.addAction(&newBucketAction);
	}

	menu.addAction(&deleteAction);
	if (m_treeView->selectionModel()->selectedRows().count() == 0) {
		// We could also disable the delete action for other conditions
		// (selecting a folder object), however, we don't currently
		// have a good way to inform the user why it's disabled.
		// Maybe if we had a delete action to the toolbar, we can
		// add a tooltip but we can't add a tooltip to a QAction that's
		// in a QMenu.
		deleteAction.setEnabled(false);
	}

	QAction* selectedAction = menu.exec(QCursor::pos());
	if (!selectedAction) {
		return;
	}

	if (selectedAction == &newBucketAction) {
		CreateBucket();
	} else if (selectedAction == &deleteAction) {
		DeleteSelected();
	}
}

void
DS3Browser::OnModelItemDoubleClick(const QModelIndex& index)
{
	if (m_model->IsBucketOrFolder(index)) {
		QString path = IndexToPath(index);
		m_treeView->setRootIndex(index);
		UpdatePathLabel(path);
	}
}

void
DS3Browser::Refresh()
{
	QModelIndex index = m_treeView->rootIndex();
	if (m_model->IsFetching(index)) {
		return;
	}

	m_model->Refresh(index);
	QString path = IndexToPath(index);
	UpdatePathLabel(path);
	m_treeView->setRootIndex(index);
}

void
DS3Browser::OnModelItemClick(const QModelIndex& index)
{
	if (m_model->IsPageBreak(index)) {
		m_model->fetchMore(index.parent());
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
DS3Browser::DeleteSelected()
{
	QModelIndexList selectedIndexes = m_treeView->selectionModel()->selectedRows();
	if (selectedIndexes.count() == 0) {
		LOG_ERROR("Nothing selected to delete");
		LOG_FILE("ERROR:       DELETE OBJECT failed, nothing selected to delete");
		return;
	} else if (selectedIndexes.count() > 1) {
		LOG_ERROR("Deleting more than one item at a time is not supported");
		LOG_FILE("ERROR:       DELETE OBJECTS failed, deleting more than one item at a time is not supported");
		return;
	}

	QModelIndex selectedIndex = selectedIndexes[0];
	if (m_model->IsFolder(selectedIndex)) {
		LOG_ERROR("Deleting folders is not yet supported");
		LOG_FILE("ERROR:       DELETE OBJECT failed, deleting folders is not yet supported");
		return;
	}

	QString name = m_model->GetFullName(selectedIndex);
	DS3DeleteDialog* dialog;
	if (m_model->IsBucket(selectedIndex)) {
		dialog = new DeleteBucketDialog(m_client, name);
	} else {
		QString bucketName = m_model->GetBucketName(selectedIndex);
		dialog = new DeleteObjectsDialog(m_client, bucketName, QStringList(name));
	}
	if (dialog->exec() == QDialog::Accepted) {
		Refresh();
	}
	delete dialog;
}

bool
DS3Browser::IsBucketSelectedOnly() const
{
	QModelIndexList selectedIndexes = m_treeView->selectionModel()->selectedRows(0);
	bool bucketSelected = false;
	if (selectedIndexes.count() == 1) {
		QModelIndex selectedIndex = selectedIndexes[0];
		if (m_model->IsBucket(selectedIndex)) {
			bucketSelected = true;
		}
	}
	return bucketSelected;
}
