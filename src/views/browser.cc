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

#include "views/browser.h"
#include "views/browser_tree_view_style.h"

Browser::Browser(QWidget* parent, Qt::WindowFlags flags)
	: QWidget(parent, flags),
	  m_layout(new QVBoxLayout(this)),
	  m_path(new QLabel("/")),
	  m_toolBar(new QToolBar),
	  m_treeView(new QTreeView)
{
	AddToolBarActions();

	m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_treeView, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(OnContextMenuRequested(const QPoint&)));

	m_treeView->setExpandsOnDoubleClick(false);
	m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	connect(m_treeView, SIGNAL(doubleClicked(const QModelIndex&)),
		this, SLOT(OnModelItemDoubleClick(const QModelIndex&)));

	m_treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_treeView->setDragDropMode(QAbstractItemView::DragDrop);
	m_treeView->setDropIndicatorShown(true);
	m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

	// Remove the focus rectangle around the tree view on OSX.
	m_treeView->setAttribute(Qt::WA_MacShowFocusRect, 0);

	m_treeViewStyle = new BrowserTreeViewStyle(style());
	m_treeView->setStyle(m_treeViewStyle);

	m_layout->setContentsMargins (0, 0, 0, 0);
	m_layout->addWidget(m_toolBar);
	m_layout->addWidget(m_path);
	m_layout->addWidget(m_treeView);

	setLayout(m_layout);
}

Browser::~Browser()
{
}

void
Browser::AddToolBarActions()
{
	m_parentAction = new QAction(style()->standardIcon(QStyle::SP_FileDialogToParent),
				     "Parent directory", this);
	connect(m_parentAction, SIGNAL(triggered()), this, SLOT(GoToParent()));
	m_toolBar->addAction(m_parentAction);

	m_rootAction = new QAction(style()->standardIcon(QStyle::SP_ComputerIcon),
				   "Root directory", this);
	connect(m_rootAction, SIGNAL(triggered()), this, SLOT(GoToRoot()));
	m_toolBar->addAction(m_rootAction);
}

void
Browser::UpdatePathLabel(const QString& path)
{
	m_path->setText(path);
}

void
Browser::GoToParent()
{
	QModelIndex parentIndex = m_treeView->rootIndex().parent();
	QString parentPath = IndexToPath(parentIndex);
	m_treeView->setRootIndex(parentIndex);
	UpdatePathLabel(parentPath);
}

void
Browser::GoToRoot()
{
	QModelIndex rootIndex;
	m_treeView->setRootIndex(rootIndex);
	UpdatePathLabel(IndexToPath(rootIndex));
}
