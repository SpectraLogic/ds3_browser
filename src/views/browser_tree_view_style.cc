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

#include <QTreeView>

#include "views/browser_tree_view_style.h"

BrowserTreeViewStyle::BrowserTreeViewStyle(QStyle* style)
	: QProxyStyle(style)
{
}

void
BrowserTreeViewStyle::drawPrimitive(PrimitiveElement element,
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
	} else {
		QProxyStyle::drawPrimitive(element, option, painter, widget);
	}
}
