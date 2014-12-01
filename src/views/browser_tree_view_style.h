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

#ifndef BROWSER_TREE_VIEW_STYLE_H
#define BROWSER_TREE_VIEW_STYLE_H

#include <QProxyStyle>
#include <QStyleOption>

// A custom DS3/Host Browser QTreeView style to override how the drag/drop
// indicator rectangle is drawn.  The default is to draw a rectangle around the
// table cell that the dragged item is over.  This class instead draws a
// rectangle around the entire row with the left-edge starting at the correct
// indentation level.
class BrowserTreeViewStyle : public QProxyStyle {
public:
	BrowserTreeViewStyle(QStyle* style = 0);

	void drawPrimitive (PrimitiveElement element,
			    const QStyleOption* option,
			    QPainter* painter,
			    const QWidget* widget = 0 ) const;
};

#endif
