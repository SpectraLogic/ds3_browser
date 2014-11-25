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

#include <QDragEnterEvent>
#include <QMimeData>
#include <QMenu>
#include <QProxyStyle>

#include "lib/client.h"
#include "lib/logger.h"
#include "models/ds3_browser_model.h"
#include "models/session.h"
#include "views/buckets/new_bucket_dialog.h"
#include "views/ds3_browser.h"
#include "views/jobs_view.h"

// A custom DS3Browser QTreeView style to override how the drag/drop indicator
// rectangle is drawn.  The default is to draw a rectangle around the table
// cell that the dragged item is over.  This class instead draws a rectangle
// around the entire row with the left-edge starting at the correct indentation
// level.
class DS3BrowserTreeViewStyle : public QProxyStyle {
public:
	DS3BrowserTreeViewStyle(QStyle* style = 0);

	void drawPrimitive (PrimitiveElement element,
			    const QStyleOption* option,
			    QPainter* painter,
			    const QWidget* widget = 0 ) const;
};

DS3BrowserTreeViewStyle::DS3BrowserTreeViewStyle(QStyle* style)
	: QProxyStyle(style)
{
}

void
DS3BrowserTreeViewStyle::drawPrimitive(PrimitiveElement element,
				       const QStyleOption* option,
				       QPainter* painter,
				       const QWidget* widget) const
{
	if (element == QStyle::PE_IndicatorItemViewItemDrop &&
	    !option->rect.isNull()) {
		QStyleOption opt(*option);
		if (widget) {
			const QTreeView* treeView = static_cast<const QTreeView*>(widget);
			QPoint cellCenter = opt.rect.center();
			QModelIndex cell = treeView->indexAt(cellCenter);
			QModelIndex column0Cell = treeView->model()->index(cell.row(), 0, cell.parent());
			QRect column0CellRect = treeView->visualRect(column0Cell);
			opt.rect.setLeft(column0CellRect.left());
			opt.rect.setRight(widget->width());
		}
		QProxyStyle::drawPrimitive(element, &opt, painter, widget);
		return;
	}
	QProxyStyle::drawPrimitive(element, option, painter, widget);
}

DS3Browser::DS3Browser(Session* session, JobsView* jobsView,
		       QWidget* parent, Qt::WindowFlags flags)
	: Browser(parent, flags),
	  m_jobsView(jobsView)
{
	AddCustomToolBarActions();

	m_client = new Client(session);
	m_model = new DS3BrowserModel(m_client, this);
	m_model->SetView(m_treeView);
	m_treeView->setModel(m_model);

	m_treeView->setDragDropMode(QAbstractItemView::DropOnly);
	m_treeView->setDropIndicatorShown(true);
	m_treeViewStyle = new DS3BrowserTreeViewStyle(style());
	m_treeView->setStyle(m_treeViewStyle);

	connect(m_treeView, SIGNAL(clicked(const QModelIndex&)),
		this, SLOT(OnModelItemClick(const QModelIndex&)));

	connect(m_client, SIGNAL(JobProgressUpdate(const Job)),
		m_jobsView, SLOT(UpdateJob(const Job)));
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
