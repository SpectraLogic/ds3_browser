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

#ifndef DS3_BROWSER_H
#define DS3_BROWSER_H

#include <QStringList>
#include <QLineEdit>

#include "lib/watchers/get_bucket_watcher.h"
#include "lib/watchers/get_service_watcher.h"
#include "models/job.h"
#include "views/browser.h"

class DS3BrowserItem;
class DS3BrowserModel;
class DS3SearchModel;
class DS3SearchTree;
class JobsView;

// DS3Browser, a Browser class used for browsing a DS3 system (e.g a
// BlackPearl)
class DS3Browser : public Browser
{
	Q_OBJECT

public:
	DS3Browser(Client* client,
		   JobsView* jobsView,
		   QWidget* parent = 0,
		   Qt::WindowFlags flags = 0);
	bool CanReceive(QModelIndex& index);
	void CanTransfer(bool enable);
	QModelIndexList GetSelected();
	void GetData(QMimeData* data);
	void SetViewRoot(const QModelIndex& index);

signals:
	void Transferable();
	void StartTransfer(QMimeData* data);

public slots:
	void HandleJobUpdate(const Job job);

protected:
	void AddCustomToolBarActions();
	QString IndexToPath(const QModelIndex& index) const;
	void OnContextMenuRequested(const QPoint& pos);
	void OnModelItemDoubleClick(const QModelIndex& index);

	QAction* m_rootAction;
	QAction* m_refreshAction;
	QAction* m_searchAction;
	QLineEdit* m_searchBar;
	QAction* m_transferAction;

protected slots:
	void BeginSearch();
	void RunSearch();
	void Refresh();
	void OnModelItemClick(const QModelIndex& index);
	void CreateSearchTree(bool found);
	void PrepareTransfer();

private:
	void CreateBucket();
	void CreateFolder();
	void DeleteSelected();
	bool IsBucketSelectedOnly() const;

	DS3BrowserModel* m_model;
	DS3SearchModel* m_searchModel;
	DS3SearchTree* m_searchView;
	JobsView* m_jobsView;
};


class DS3SearchTree : public QTreeView
{
public:
	DS3SearchTree();
};


#endif
