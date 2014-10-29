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

#ifndef DS3_BROWSER_MODEL_H
#define DS3_BROWSER_MODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <ds3.h>

class Client;
class DS3BrowserItem;

class DS3BrowserModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	DS3BrowserModel(Client* client, QObject* parent = 0);
	~DS3BrowserModel();

	QModelIndex index(int row, int column = 0,
			  const QModelIndex &parent = QModelIndex()) const;

	QModelIndex parent(const QModelIndex &index) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index,
		int role = Qt::DisplayRole) const;

	QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const;

	bool hasChildren(const QModelIndex & parent = QModelIndex()) const;

	void Refresh();

private:
	Client* m_client;

	DS3BrowserItem* m_rootItem;
};

#endif
