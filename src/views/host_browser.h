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

#ifndef HOST_BROWSER_H
#define HOST_BROWSER_H

#include <QFileSystemModel>
#include <QList>

#include "views/browser.h"

// HostBrowser, a Browser class used for the local/host system
class HostBrowser : public Browser
{
	Q_OBJECT

public:
	HostBrowser(QWidget* parent = 0, Qt::WindowFlags flags = 0);

protected:
	void AddCustomToolBarActions();
	QString IndexToPath(const QModelIndex& index);
	void UpdatePathLabel(const QString& path);
	void OnContextMenuRequested(const QPoint& pos);
	void OnModelItemDoubleClick(const QModelIndex& index);

private:
	QAction* m_homeAction;
	QAction* m_uploadAction;
	QFileSystemModel* m_model;

	QList<QString> GetSelectedFiles();

private slots:
	void GoToHome();
};

#endif
