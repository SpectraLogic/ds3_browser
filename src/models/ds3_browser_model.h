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
#include <QTreeView>
#include <ds3.h>

class Client;
class DS3BrowserItem;

class DS3BrowserModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	DS3BrowserModel(Client* client, QObject* parent = 0);
	~DS3BrowserModel();

	// QAbstractItemModel Overrides

	// DS3BrowserModel only uses canFetchMore so QAbstractItemView can
	// automatically handle the first page load.  In order to prevent it
	// from loading every page right away, this isn't used past that.
	bool canFetchMore(const QModelIndex& parent) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index,
		      int role = Qt::DisplayRole) const;
	bool dropMimeData(const QMimeData* data,
			  Qt::DropAction action,
			  int row, int column,
			  const QModelIndex& parent);
	Qt::ItemFlags flags(const QModelIndex& index) const;
	void fetchMore(const QModelIndex& parent);
	bool hasChildren(const QModelIndex & parent = QModelIndex()) const;
	QVariant headerData(int section, Qt::Orientation orientation,
			    int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column = 0,
			  const QModelIndex &parent = QModelIndex()) const;
	QMimeData* mimeData(const QModelIndexList& indexes) const;
	QStringList mimeTypes() const;
	QModelIndex parent(const QModelIndex &index) const;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	int rowCount(const QModelIndex &parent = QModelIndex()) const;

	// Non-Qt Methods

	bool IsBucketOrFolder(const QModelIndex& index) const;
	bool IsPageBreak(const QModelIndex& index) const;
	bool IsFetching(const QModelIndex& parent) const;
	QString GetPath(const QModelIndex& index) const;
	void Refresh(const QModelIndex& rootIndex = QModelIndex());
	void SetView(QTreeView* view);

public slots:
	void HandleGetServiceResponse();
	void HandleGetBucketResponse();

private:
	void FetchMoreBuckets(const QModelIndex& parent);
	void FetchMoreObjects(const QModelIndex& parent);
	DS3BrowserItem* IndexToItem(const QModelIndex& index) const;
	void RemoveLoadingItem(const QModelIndex& parent);

	Client* m_client;
	DS3BrowserItem* m_rootItem;
	QTreeView* m_view;
};

inline void
DS3BrowserModel::SetView(QTreeView* view)
{
	m_view = view;
}


inline DS3BrowserItem*
DS3BrowserModel::IndexToItem(const QModelIndex& index) const
{
	return static_cast<DS3BrowserItem*>(index.internalPointer());
}

#endif
