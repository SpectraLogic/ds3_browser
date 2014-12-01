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

#ifndef BROWSER_H
#define BROWSER_H

#include <QAction>
#include <QLabel>
#include <QModelIndex>
#include <QPoint>
#include <QToolbar>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>

class BrowserTreeViewStyle;

// Browser, an abstract class that represents a host or remote file browser
class Browser : public QWidget
{
	Q_OBJECT

public:
	Browser(QWidget* parent = 0, Qt::WindowFlags flags = 0);
	virtual ~Browser();

protected:
	void AddToolBarActions();
	virtual QString IndexToPath(const QModelIndex& index) const = 0;
	virtual void UpdatePathLabel(const QString& path);

	QVBoxLayout* m_layout;
	QLabel* m_path;
	QToolBar* m_toolBar;
	QTreeView* m_treeView;
	BrowserTreeViewStyle* m_treeViewStyle;

	QAction* m_parentAction;
	QAction* m_rootAction;

protected slots:
	void GoToParent();
	void GoToRoot();
	virtual void OnContextMenuRequested(const QPoint& pos) = 0;
	virtual void OnModelItemDoubleClick(const QModelIndex& index) = 0;
};

#endif
