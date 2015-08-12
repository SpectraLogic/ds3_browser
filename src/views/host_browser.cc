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
#include "models/host_browser_model.h"
#include "views/host_browser.h"

HostBrowser::HostBrowser(Client* client, QWidget* parent, Qt::WindowFlags flags)
	: Browser(client, parent, flags),
	  m_model(new HostBrowserModel(client, this))
{
	AddCustomToolBarActions();

	QString rootPath = m_model->myComputer().toString();
	UpdatePathLabel(rootPath);
	m_model->setReadOnly(false);
	m_model->setRootPath(rootPath);
	m_model->setFilter(QDir::AllDirs |
			   QDir::AllEntries |
			   QDir::NoDotAndDotDot |
			   QDir::Hidden);
	m_treeView->setModel(m_model);

	connect(m_treeView, SIGNAL(clicked(const QModelIndex&)),
		this, SLOT(OnModelItemClick(const QModelIndex&)));
}

void
HostBrowser::AddCustomToolBarActions()
{
	m_rootAction = new QAction(QIcon(":/resources/icons/computer.png"),
				   "Computer", this);
	connect(m_rootAction, SIGNAL(triggered()), this, SLOT(GoToRoot()));
	m_toolBar->addAction(m_rootAction);

	m_homeAction = new QAction(QIcon(":/resources/icons/home.png"),
				   "Home directory", this);
	connect(m_homeAction, SIGNAL(triggered()), this, SLOT(GoToHome()));
	m_toolBar->addAction(m_homeAction);

	QTextCodec* codec = QTextCodec::codecForName("UTF-8");
	QString rightArrow = codec->fromUnicode("");
	m_transferAction = new QAction(rightArrow, this);
	m_transferAction->setFont(QFont("FontAwesome", 16));
	m_transferAction->setText(rightArrow);
	m_transferAction->setEnabled(false);
	m_transferAction->setToolTip("Transfer to DS3");
	connect(m_transferAction, SIGNAL(triggered()), this, SLOT(PrepareTransfer()));
	m_toolBar->addAction(m_transferAction);
}

QString
HostBrowser::IndexToPath(const QModelIndex& index) const
{
	QString path = m_model->filePath(index);
	if (path.isEmpty()) {
		path = m_model->myComputer().toString();
	}
	return path;
}

void
HostBrowser::UpdatePathLabel(const QString& path)
{
	m_path->setText(QDir::toNativeSeparators(path));
}

void
HostBrowser::GoToHome()
{
	QString path = QDir::homePath();
	m_treeView->setRootIndex(m_model->index(path));
	UpdatePathLabel(path);
}

void
HostBrowser::OnContextMenuRequested(const QPoint& /*pos*/)
{
}

void
HostBrowser::OnModelItemClick(const QModelIndex& index)
{
	QModelIndex temp = index;
	temp = temp;
	emit Transferable();
}

void
HostBrowser::OnModelItemDoubleClick(const QModelIndex& index)
{
	QString path = m_model->filePath(index);
	QDir dir = QDir(path);
	if (m_model->isDir(index) && dir.isReadable())
	{
		m_treeView->setRootIndex(index);
		UpdatePathLabel(path);
	}
}

bool
HostBrowser::CanReceive(QModelIndex& index)
{
	bool able = false;
	QModelIndexList indicies = GetSelected();
	if (indicies.isEmpty()) {
		index = m_treeView->rootIndex();
	} else if (indicies.count() == 1) {
		index = indicies[0];
	}

	if (m_model->isDir(index) && (m_model->filePath(index) != m_model->rootPath())) {
		able = true;
	}
	return able;
}

void
HostBrowser::CanTransfer(bool enable)
{
	m_transferAction->setEnabled(enable);
}

QModelIndexList
HostBrowser::GetSelected()
{
	return m_treeView->selectionModel()->selectedRows(0);
}

void
HostBrowser::PrepareTransfer()
{
	emit StartTransfer(m_model->mimeData(GetSelected()));
}

void
HostBrowser::GetData(QMimeData* data)
{
	// Row and column for this call are -1 so that the data is "dropped" directly
	//   on the parent index given
	m_model->dropMimeData(data, Qt::CopyAction, -1, -1, m_treeView->rootIndex());
}

void
HostBrowser::SetViewRoot(const QModelIndex& index)
{
	OnModelItemDoubleClick(index);
}