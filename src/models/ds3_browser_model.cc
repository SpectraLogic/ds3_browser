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

#include <QDateTime>
#include <QFuture>
#include <QIcon>
#include <QModelIndex>
#include <QRegularExpression>
#include <QSet>

#include "helpers/number_helper.h"
#include "lib/client.h"
#include "lib/logger.h"
#include "lib/mime_data.h"
#include "lib/errors/ds3_error.h"
#include "lib/watchers/get_bucket_watcher.h"
#include "lib/watchers/get_objects_watcher.h"
#include "models/ds3_browser_model.h"
#include "models/ds3_url.h"

// Must match m_rootItem->m_data;
enum Column { NAME, OWNER, SIZE_COL, KIND, CREATED, COUNT };

static const QString REST_TIMESTAMP_FORMAT = "yyyy-MM-ddThh:mm:ss.000Z";
static const QString VIEW_TIMESTAMP_FORMAT = "MMMM d, yyyy h:mm AP";

static const QString BUCKET = "Bucket";
static const QString OBJECT = "Object";
static const QString FOLDER = "Folder";

//
// DS3BrowserItem
//

class DS3BrowserItem
{
public:
	DS3BrowserItem(const QList<QVariant>& data,
		       QString bucketName = QString(),
		       QString prefix = QString(),
		       DS3BrowserItem* parent = 0);
	virtual ~DS3BrowserItem();

	void AppendChild(DS3BrowserItem* item);
	bool GetCanFetchMore() const;
	QString GetBucketName() const;
	DS3BrowserItem* GetChild(int row) const;
	void RemoveChild(int row);
	int GetChildCount() const;
	int GetColumnCount() const;
	QVariant GetData(int column) const;
	QString GetNextMarker() const;
	QString GetPrefix() const;
	int GetRow() const;
	DS3BrowserItem* GetParent() const;
	virtual bool IsLoadingItem() const;
	virtual bool IsPageBreak() const;
	bool IsFetching() const;
	void Reset();
	QString GetFullName() const;
	QString GetPath() const;

	void SetCanFetchMore(bool canFetchMore);
	void SetFetching(bool fetching);
	void SetNextMarker(const QString nextMarker);

protected:
	// m_canFetchMore only represents what DS3BrowserModel should report
	// for canFetchMore and not necessarily if the previous get
	// children request was truncated or not.
	bool m_canFetchMore;
	bool m_fetching;
	QList<DS3BrowserItem*> m_children;
	// List of data to show in the table.  Each item in the list
	// directly corresponds to a column.
	QList<QVariant> m_data;
	// So object items so they can easily keep track of what
	// bucket they're in.  For bucket items, this is the same as
	// m_data[0];
	const QString m_bucketName;
	QString m_nextMarker;
	DS3BrowserItem* m_parent;
	// All parent folder object names, not including the bucket name
	const QString m_prefix;

	QList<DS3BrowserItem*> GetChildren() const;
};

DS3BrowserItem::DS3BrowserItem(const QList<QVariant>& data,
			       QString bucketName,
			       QString prefix,
			       DS3BrowserItem* parent)
	: m_canFetchMore(true),
	  m_fetching(false),
	  m_data(data),
	  m_bucketName(bucketName),
	  m_parent(parent),
	  m_prefix(prefix)
{
}

DS3BrowserItem::~DS3BrowserItem()
{
	qDeleteAll(m_children);
}

void
DS3BrowserItem::AppendChild(DS3BrowserItem* item)
{
	m_children << item;
}

inline QString
DS3BrowserItem::GetBucketName() const
{
	return m_bucketName;
}

inline bool
DS3BrowserItem::GetCanFetchMore() const
{
	return m_canFetchMore;
}

DS3BrowserItem*
DS3BrowserItem::GetChild(int row) const
{
	return GetChildren().value(row);
}

void
DS3BrowserItem::RemoveChild(int row)
{
	if (row < m_children.count()) {
		DS3BrowserItem* item = m_children.at(row);
		m_children.removeAt(row);
		delete item;
	}
}

int
DS3BrowserItem::GetChildCount() const
{
	return GetChildren().count();	
}

inline QString
DS3BrowserItem::GetNextMarker() const
{
	return m_nextMarker;
}

inline QString
DS3BrowserItem::GetPrefix() const
{
	return m_prefix;
}

int
DS3BrowserItem::GetRow() const
{
	if (m_parent) {
		return m_parent->GetChildren().indexOf(const_cast<DS3BrowserItem*>(this));
	}

	return 0;
}

int
DS3BrowserItem::GetColumnCount() const
{
	return m_data.count();
}

QVariant
DS3BrowserItem::GetData(int column) const
{
	QVariant data = m_data.value(column);
	if (column == SIZE_COL && data != "Size" && data != "--") {
		qulonglong size = data.toULongLong();
		data = QVariant(NumberHelper::ToHumanSize(size));
	}
	return data;
}

inline DS3BrowserItem*
DS3BrowserItem::GetParent() const
{
	return m_parent;
}

inline bool
DS3BrowserItem::IsLoadingItem() const
{
	return false;
}

inline bool
DS3BrowserItem::IsPageBreak() const
{
	return false;
}

inline bool
DS3BrowserItem::IsFetching() const
{
	return m_fetching;
}

void
DS3BrowserItem::Reset()
{
	qDeleteAll(m_children);
	m_children.clear();
	m_canFetchMore = true;
	m_nextMarker = QString();
}

QString
DS3BrowserItem::GetFullName() const
{
	return (m_prefix + GetData(NAME).toString());
}

QString
DS3BrowserItem::GetPath() const
{
	QString path = "/" + m_bucketName;
	if (GetData(KIND) == BUCKET) {
		return path;
	}

	if (m_prefix.isEmpty()) {
		path += "/";
	} else {
		path += "/" + m_prefix;
	}
	path += GetData(NAME).toString();
	return path;
}

QList<DS3BrowserItem*>
DS3BrowserItem::GetChildren() const
{
	return m_children;
}

inline void
DS3BrowserItem::SetCanFetchMore(bool canFetchMore)
{
	m_canFetchMore = canFetchMore;
}

inline void
DS3BrowserItem::SetFetching(bool fetching)
{
	m_fetching = fetching;
}

inline void
DS3BrowserItem::SetNextMarker(const QString nextMarker)
{
	m_nextMarker = nextMarker;
}

class PageBreakItem : public DS3BrowserItem
{
public:
	PageBreakItem(DS3BrowserItem* parent = 0);

	bool IsPageBreak() const;
};

PageBreakItem::PageBreakItem(DS3BrowserItem* parent)
	: DS3BrowserItem(QList<QVariant>(), "", "", parent)
{
	m_data << "Click to load more";
}

inline bool
PageBreakItem::IsPageBreak() const
{
	return true;
}

class LoadingItem : public DS3BrowserItem
{
public:
	LoadingItem(DS3BrowserItem* parent = 0);

	bool IsLoadingItem() const;
};

LoadingItem::LoadingItem(DS3BrowserItem* parent)
	: DS3BrowserItem(QList<QVariant>(), "", "", parent)
{
	m_data << "Loading ...";
}

inline bool
LoadingItem::IsLoadingItem() const
{
	return true;
}

//
// DS3BrowserModel
//

DS3BrowserModel::DS3BrowserModel(Client* client, QObject* parent)
	: QAbstractItemModel(parent),
	  m_client(client)
{
	// Headers must match DS3BrowserModel::Column
	QList<QVariant> column_names;
	column_names << "Name" << "Owner" << "Size" << "Kind" << "Created";
	m_rootItem = new DS3BrowserItem(column_names);
}

DS3BrowserModel::~DS3BrowserModel()
{
	delete m_rootItem;
}

bool
DS3BrowserModel::canFetchMore(const QModelIndex& parent) const
{
	DS3BrowserItem* parentItem;
	if (parent.isValid()) {
		parentItem = IndexToItem(parent);
	} else {
		parentItem = m_rootItem;
	}
	return parentItem->GetCanFetchMore();
}

int
DS3BrowserModel::columnCount(const QModelIndex& parent) const
{
	DS3BrowserItem* item;
	if (parent.isValid()) {
		item = IndexToItem(parent);
	} else {
		item = m_rootItem;
	}
	return item->GetColumnCount();
}

QVariant
DS3BrowserModel::data(const QModelIndex &index, int role) const
{
	DS3BrowserItem* item;
	QVariant data;

	if (!index.isValid()) {
		return data;
	}

	item = IndexToItem(index);
	int column = index.column();

	switch (role)
	{
	case Qt::DisplayRole:
		data = item->GetData(column);
		if (column == 0 && (item->IsPageBreak() || item->IsLoadingItem())) {
			m_view->setFirstColumnSpanned(index.row(), index.parent(), true);
		}
		break;
	case Qt::DecorationRole:
		if (column == NAME) {
			QVariant kind = item->GetData(KIND);
			if (kind == BUCKET) {
				data = QIcon(":/resources/icons/bucket.png");
			} else if (kind == FOLDER) {
				data = QIcon(":/resources/icons/files.png");
			} else if (kind == OBJECT) {
				data = QIcon(":/resources/icons/file.png");
			}
		}
		break;
	}
	return data;
}

bool
DS3BrowserModel::dropMimeData(const QMimeData* data,
			      Qt::DropAction action,
			      int row, int column,
			      const QModelIndex& parentIndex)
{
	if (!data->hasUrls()) {
		return QAbstractItemModel::dropMimeData(data, action, row,
							column, parentIndex);
	}

	DS3BrowserItem* parent = IndexToItem(parentIndex);
	QString bucketName = parent->GetBucketName();
	QString prefix = parent->GetPrefix();
	if (parent->GetData(KIND) != BUCKET) {
		prefix += parent->GetData(NAME).toString();
	}
	prefix.replace(QRegularExpression("^/"), "");
	QList<QUrl> urls = data->urls();
	m_client->BulkPut(bucketName, prefix, urls);
	return true;
}

Qt::ItemFlags
DS3BrowserModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags flags = QAbstractItemModel::flags(index);
	if (index.isValid()) {
		DS3BrowserItem* item = IndexToItem(index);
		QVariant kind = item->GetData(KIND);
		flags |= Qt::ItemIsDragEnabled;
		if (kind == BUCKET || kind == FOLDER) {
			flags |= Qt::ItemIsDropEnabled;
		}
	}
	return flags;
}

void
DS3BrowserModel::fetchMore(const QModelIndex& parent)
{
	bool parentIsValid = parent.isValid();
	DS3BrowserItem* parentItem;
	if (parent.isValid()) {
		parentItem = IndexToItem(parent);
	} else {
		parentItem = m_rootItem;
	}

	int lastRow = parentItem->GetChildCount() - 1;

	DS3BrowserItem* loadingItem = new LoadingItem(parentItem);
	int loadingItemRow = lastRow >= 0 ? lastRow : 0;
	beginInsertRows(parent, loadingItemRow, loadingItemRow);
	parentItem->AppendChild(loadingItem);
	endInsertRows();

	if (lastRow >= 0) {
		DS3BrowserItem* lastChildItem = parentItem->GetChild(lastRow);
		if (lastChildItem->IsPageBreak()) {
			removeRow(lastRow, parent);
		}
	}


	parentItem->SetFetching(true);
	parentIsValid ? FetchMoreObjects(parent) : FetchMoreBuckets(parent);

	// Always set CanFetchMore to false so the view doesn't automatically
	// come right back around and ask to fetchMore when this model
	// emits the rowsInserted signal (which is the way
	// QAbstractItemView handles fetchMore).
	parentItem->SetCanFetchMore(false);
}

// rowCount actually determines whether or not the bucket has any objects in
// it.  hasChildren always returns true for buckets and folders so the
// caret is always displayed even if we don't yet know if the bucket/folder
// has any objects.
bool
DS3BrowserModel::hasChildren(const QModelIndex& parent) const
{
	if (!parent.isValid()) {
		return true;
	}

	DS3BrowserItem* item = IndexToItem(parent);
	QVariant kind = item->GetData(KIND);
	return (kind == BUCKET || kind == FOLDER);
}

QVariant
DS3BrowserModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
	if (role == Qt::DisplayRole) {
		return m_rootItem->GetData(section);
	}
	return QVariant();
}

QModelIndex
DS3BrowserModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	DS3BrowserItem* parentItem;

	if (parent.isValid()) {
		parentItem = IndexToItem(parent);
	} else {
		parentItem = m_rootItem;
	}

	DS3BrowserItem* childItem = parentItem->GetChild(row);
	if (childItem) {
		return createIndex(row, column, childItem);
	} else {
		return QModelIndex();
	}
}

QMimeData*
DS3BrowserModel::mimeData(const QModelIndexList& indexes) const
{
	if (indexes.isEmpty()) {
		return 0;
	}

	QList<QUrl> urls;
	QString endpoint = m_client->GetEndpoint();
	for (int i = 0; i < indexes.size(); i++) {
		QModelIndex index = indexes.at(i);
		if (index.column() == 0) {
			DS3BrowserItem* item = IndexToItem(index);
			QString path = item->GetPath();
			QVariant kind = item->GetData(KIND);
			if (kind == FOLDER && !path.endsWith("/")) {
				path += "/";
			}
			DS3URL url(endpoint, path);
			urls << QUrl(url);
		}
	}
	MimeData* mimeData = new MimeData;
	mimeData->SetDS3URLs(urls);
	return mimeData;
}

QStringList
DS3BrowserModel::mimeTypes() const
{
	QStringList types;
	types << "text/uri-list";
	return types;
}

QModelIndex
DS3BrowserModel::parent(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return QModelIndex();
	}

	DS3BrowserItem* childItem = IndexToItem(index);
	DS3BrowserItem* parentItem = childItem->GetParent();

	if (parentItem == m_rootItem) {
		return QModelIndex();
	}

	return createIndex(parentItem->GetRow(), 0, parentItem);
}

bool
DS3BrowserModel::removeRows(int row, int count, const QModelIndex& parent)
{
	if (row < 0 || count <= 0 || (row + count) > rowCount(parent)) {
		return false;
	}

	DS3BrowserItem* parentItem;
	if (parent.isValid()) {
		parentItem = IndexToItem(parent);
	} else {
		parentItem = m_rootItem;
	}

	beginRemoveRows(parent, row, row + count - 1);

	for (int i = 0; i < count; i++) {
		parentItem->RemoveChild(row + i);
	}

	endRemoveRows();

	return true;
}

int
DS3BrowserModel::rowCount(const QModelIndex &parent) const
{
	DS3BrowserItem* parentItem;
	if (parent.column() > 0) {
		return 0;
	}

	if (parent.isValid()) {
		parentItem = IndexToItem(parent);
	} else {
		parentItem = m_rootItem;
	}

	return parentItem->GetChildCount();
}

bool
DS3BrowserModel::IsBucket(const QModelIndex& index) const
{
	DS3BrowserItem* item = IndexToItem(index);
	QVariant kind = item->GetData(KIND);
	return (kind == BUCKET);
}

bool
DS3BrowserModel::IsFolder(const QModelIndex& index) const
{
	DS3BrowserItem* item = IndexToItem(index);
	QVariant kind = item->GetData(KIND);
	return (kind == FOLDER);
}

bool
DS3BrowserModel::IsBucketOrFolder(const QModelIndex& index) const
{
	DS3BrowserItem* item = IndexToItem(index);
	QVariant kind = item->GetData(KIND);
	return (kind == BUCKET || kind == FOLDER);
}

bool
DS3BrowserModel::IsPageBreak(const QModelIndex& index) const
{
	DS3BrowserItem* item = IndexToItem(index);
	return item->IsPageBreak();
}

bool
DS3BrowserModel::IsFetching(const QModelIndex& parent) const
{
	DS3BrowserItem* parentItem;
	if (parent.isValid()) {
		parentItem = IndexToItem(parent);
	} else {
		parentItem = m_rootItem;
	}
	return parentItem->IsFetching();
}

QString
DS3BrowserModel::GetBucketName(const QModelIndex& index) const
{
	QString name;
	DS3BrowserItem* item = IndexToItem(index);
	if (item != NULL) {
		name = item->GetBucketName();
	}
	return name;
}

QString
DS3BrowserModel::GetName(const QModelIndex& index) const
{
	QString name;
	DS3BrowserItem* item = IndexToItem(index);
	if (item != NULL) {
		name = item->GetData(NAME).toString();
	}
	return name;
}

QString
DS3BrowserModel::GetFullName(const QModelIndex& index) const
{
	QString name;
	DS3BrowserItem* item = IndexToItem(index);
	if (item != NULL) {
		name = item->GetFullName();
	}
	return name;
}

QString
DS3BrowserModel::GetPath(const QModelIndex& index) const
{
	QString path = "/";
	DS3BrowserItem* item = IndexToItem(index);
	if (item != NULL) {
		path = item->GetPath();
	}
	return path;
}

void
DS3BrowserModel::Refresh(const QModelIndex& index)
{
	DS3BrowserItem* item;
	if (index.isValid()) {
		item = IndexToItem(index);
	} else {
		item = m_rootItem;
	}

	beginResetModel();
	item->Reset();
	endResetModel();
}

void
DS3BrowserModel::FetchMoreBuckets(const QModelIndex& parent)
{
	GetServiceWatcher* watcher = new GetServiceWatcher(parent);
	connect(watcher, SIGNAL(finished()), this, SLOT(HandleGetServiceResponse()));
	QFuture<ds3_get_service_response*> future = m_client->GetService();
	watcher->setFuture(future);
}

void
DS3BrowserModel::FetchMoreObjects(const QModelIndex& parent)
{
	DS3BrowserItem* parentItem;
	if (parent.isValid()) {
		parentItem = IndexToItem(parent);
	} else {
		// Should never get here since we should never try to
		// fetch objects at the root level.
		parentItem = m_rootItem;
	}

	QString bucketName = parentItem->GetBucketName();
	QString prefix = parentItem->GetPrefix();
	bool isBucket = parentItem->GetData(KIND) == BUCKET;
	if (!isBucket) {
		prefix += parentItem->GetData(NAME).toString() + "/";
	}
	QString nextMarker = parentItem->GetNextMarker();

	GetBucketWatcher* watcher = new GetBucketWatcher(parent,
							 bucketName,
							 prefix);
	connect(watcher, SIGNAL(finished()), this, SLOT(HandleGetBucketResponse()));
	QFuture<ds3_get_bucket_response*> future = m_client->GetBucket(bucketName,
								       prefix,
								       nextMarker);
	watcher->setFuture(future);
}

void
DS3BrowserModel::HandleGetServiceResponse()
{
	LOG_DEBUG("HandleGetServiceResponse");

	GetServiceWatcher* watcher = static_cast<GetServiceWatcher*>(sender());
	const QModelIndex& parent = watcher->GetParentModelIndex();

	DS3BrowserItem* parentItem;
	if (parent.isValid()) {
		// Should never get here since we should never try to fetch
		// buckets at the bucket level.
		parentItem = IndexToItem(parent);
	} else {
		parentItem = m_rootItem;
	}

	ds3_get_service_response* response = 0;
	try {
		response = watcher->result();
	}
	catch (DS3Error& e) {
		LOG_ERROR("ERROR:       LIST BUCKETS failed, "+e.ToString());
	}

	size_t numBuckets = 0;
	if (response) {
		numBuckets = response->num_buckets;
	}
	int startRow = 0;
	if (numBuckets > 0) {
		startRow = rowCount(parent);
		beginInsertRows(parent, startRow, startRow + numBuckets - 1);
	}

	if (response) {
		QString owner = QString::fromUtf8(response->owner->name->value);
		for (size_t i = 0; i < response->num_buckets; i++) {
			QString name;
			char* rawCreated;
			QDateTime createdDT;
			QString created;
			// The order in which bucketData is filled must match
			// Column
			QList<QVariant> bucketData;
			DS3BrowserItem* bucket;

			ds3_bucket rawBucket = response->buckets[i];

			name = QString::fromUtf8(rawBucket.name->value);
			bucketData << name;
			bucketData << owner;
			bucketData << "--";
			bucketData << BUCKET;

			rawCreated = rawBucket.creation_date->value;
			createdDT = QDateTime::fromString(QString::fromUtf8(rawCreated),
							  REST_TIMESTAMP_FORMAT);
			created = createdDT.toString(VIEW_TIMESTAMP_FORMAT);
			bucketData << created;

			bucket = new DS3BrowserItem(bucketData,
						    name,
						    QString(),
						    parentItem);
			parentItem->AppendChild(bucket);
		}
	}

	if (numBuckets > 0) {
		endInsertRows();
	}

	int loadingRow = startRow > 0 ? startRow - 1 : 0;
	DS3BrowserItem* loadingItem = parentItem->GetChild(loadingRow);
	if (loadingItem && loadingItem->IsLoadingItem()) {
		removeRow(loadingRow, parent);
	}

	if (response) {
		ds3_free_service_response(response);
	}
	delete watcher;
	parentItem->SetFetching(false);
}

void
DS3BrowserModel::HandleGetBucketResponse()
{
	LOG_DEBUG("HandleGetBucketResponse");

	GetBucketWatcher* watcher = static_cast<GetBucketWatcher*>(sender());
	ds3_get_bucket_response* response = NULL;
	const QString& bucketName = watcher->GetBucketName();
	try {
		response = watcher->result();
	}
	catch (DS3Error& e) {
		QString msg;
		if (e.GetStatusCode() == 404) {
			msg = "Bucket \"" + bucketName + "\" does not exist";
		} else {
			msg = e.ToString();
		}
		LOG_ERROR("ERROR:       LIST OBJECTS failed, "+msg);
	}

	const QModelIndex& parent = watcher->GetParentModelIndex();
	DS3BrowserItem* parentItem;
	if (parent.isValid()) {
		parentItem = IndexToItem(parent);
	} else {
		// Should never get here since we should never try to
		// fetch objects at the root level.
		parentItem = m_rootItem;
	}

	QList<DS3BrowserItem*> newChildren;
	if (response != NULL) {
		QSet<QString> currentCommonPrefixNames;
		for (int i = 0; i < parentItem->GetChildCount(); i++) {
			DS3BrowserItem* object = parentItem->GetChild(i);
			if (object->GetData(KIND) == FOLDER) {
				currentCommonPrefixNames << object->GetData(NAME).toString();
			}
		}

		const QString& prefix = watcher->GetPrefix();
		QVariant owner = parentItem->GetData(OWNER);

		for (size_t i = 0; i < response->num_common_prefixes; i++) {
			ds3_str* rawCommonPrefix = response->common_prefixes[i];
			// The order in which bucketData is filled must match
			// Column
			QList<QVariant> objectData;
			DS3BrowserItem* object;

			QString nextName = QString::fromUtf8(rawCommonPrefix->value);
			nextName.replace(QRegularExpression("^" + prefix), "");
			nextName.replace(QRegularExpression("/$"), "");
			if (!currentCommonPrefixNames.contains(nextName)) {
				objectData << nextName;
				objectData << owner;
				objectData << "--";
				objectData << FOLDER;
				objectData << "--";
				object = new DS3BrowserItem(objectData,
							    bucketName,
							    prefix,
							    parentItem);
				newChildren << object;
			}
		}

		for (size_t i = 0; i < response->num_objects; i++) {
			QString nextName;
			char* rawCreated;
			QDateTime createdDT;
			QString created;
			// The order in which bucketData is filled must match
			// Column
			QList<QVariant> objectData;
			DS3BrowserItem* object;

			ds3_object rawObject = response->objects[i];

			nextName = QString::fromUtf8(rawObject.name->value);
			if (nextName == prefix) {
				continue;
			}
			nextName.replace(QRegularExpression("^" + prefix), "");
			objectData << nextName;

			objectData << owner;

			objectData << (quint64)rawObject.size;

			objectData << OBJECT;

			if (rawObject.last_modified) {
				rawCreated = rawObject.last_modified->value;
				createdDT = QDateTime::fromString(QString::fromUtf8(rawCreated),
								  REST_TIMESTAMP_FORMAT);
				created = createdDT.toString(VIEW_TIMESTAMP_FORMAT);
			}
			objectData << created;

			object = new DS3BrowserItem(objectData,
						    bucketName,
						    prefix,
						    parentItem);
			newChildren << object;
		}

		if (response->next_marker) {
			parentItem->SetNextMarker(QString::fromUtf8(response->next_marker->value));
		}

		if (response->is_truncated) {
			DS3BrowserItem* pageBreak = new PageBreakItem(parentItem);
			newChildren << pageBreak;
		}
	}

	int numNewChildren = newChildren.count();
	int startRow = 0;
	if (numNewChildren > 0) {
		startRow = rowCount(parent);
		beginInsertRows(parent, startRow, startRow + numNewChildren - 1);
		for (int i = 0; i < numNewChildren; i++) {
			parentItem->AppendChild(newChildren.at(i));
		}
		endInsertRows();
	}

	int loadingRow = startRow > 0 ? startRow - 1 : 0;
	DS3BrowserItem* loadingItem = parentItem->GetChild(loadingRow);
	if (loadingItem && loadingItem->IsLoadingItem()) {
		removeRow(loadingRow, parent);
	}

	if (response != NULL) {
		ds3_free_bucket_response(response);
	}
	delete watcher;
	parentItem->SetFetching(false);
}

// Model for searches
DS3SearchModel::DS3SearchModel(Client* client, QObject* parent)
	: DS3BrowserModel(client, parent),
	  m_activeSearchCount(0),
	  m_searchFoundCount(0)
{
}

// This function makes sure all data isn't fetched for the search tree, just what is searched for. When
//   a tree is created it calls the fetchMore function, so this function doesn't do anything for
//   the search tree.
void
DS3SearchModel::fetchMore(const QModelIndex& parent)
{
	// Have to do something with parent for compiling, and since
	//   parent is invalid, this doesn't affect the logs. Initially
	//   I tried return here, but a vfork() segfaults after a while
	//   with that.
	if(parent.isValid())
		return;
	else
		return;
}

// This function adds retrieved search objects from the DS3 model and puts them in the search model
void
DS3SearchModel::AppendItem(ds3_search_object* obj, QString bucketName) {
	// Data to go into new item in model
	QList<QVariant> data;

	// Checks search results, bucketName!="" means files were found
	if(bucketName != QString("")) {
		QString name;
		if(obj->name != NULL)
			name = "/"+bucketName+QString("/")+obj->name->value;
		else
			name = QString("");
		data << name;
		if(obj->owner != NULL && obj->owner->name != NULL)
			data << obj->owner->name->value;
		else
			data << QString("");
		qulonglong size = obj->size;
		if(size != 0)
			data << size;
		else
			data << QString("--");
		if(name == "/"+bucketName+"/")
			data << QString("Bucket");
		else if(name.endsWith("/"))
			data << QString("Folder");
		else if(bucketName != QString(""))
			data << QString("Object");
		else
			data << QString("");
		if(obj->last_modified != NULL)
			data << QDateTime::fromString(QString::fromUtf8(obj->last_modified->value), REST_TIMESTAMP_FORMAT).toString(VIEW_TIMESTAMP_FORMAT);
		else
			data << QString("");
	// Search results were empty, so only need these values
	} else {
		data << obj->name->value << QString("") << QString("--");
	}
	// Create the new item
	DS3BrowserItem* newItem = new DS3BrowserItem(data, QString(""), QString(""), m_rootItem);
	// Append it to the root
	m_rootItem->AppendChild(newItem);
}

void
DS3SearchModel::Search(const QModelIndex& index, QString bucket, QString prefix, QString search)
{
	// This case is for initial bucket search when no common prefixes are known. These searches
	//   call this function again but with search = "".
	if(search != "") {
		// Setting prefix to this since buckets are only at the root directory
		GetObjectsWatcher* watcher = new GetObjectsWatcher(index,
								   bucket,
								   prefix,
								   NULL);
		connect(watcher, SIGNAL(finished()), this, SLOT(HandleGetObjectsResponse()));
		QFuture<ds3_get_objects_response*> future = m_client->GetObjects(bucket,
									         "",
									         search,
									         NO_TYPE,
									         "");
		watcher->setFuture(future);
	}
}

// This function retrieves all of the bucket names and calls the search function on each bucket
void
DS3SearchModel::HandleGetServiceResponse(QString search, QTreeView* tree, DS3BrowserModel* model, GetServiceWatcher* watcher)
{
	// Set these class variables so that the model and tree that the search is being done on
	//   can be used for indices and object names
	m_searchedTree = tree;
	m_searchedModel = model;
	// Set index to the root index of the searched tree
	QModelIndex index = m_searchedTree->rootIndex();

	ds3_get_service_response* response = 0;
	try {
		response = watcher->result();
	}
	catch (DS3Error& e) {
		LOG_ERROR("Error listing buckets - " + e.ToString());
	}
	// Checks to make sure that there is a response and that the search isn't empty
	if (response && search != "") {
		// Iterate through buckets
		for (size_t i = 0; i < response->num_buckets; i++) {
			QString name;
			ds3_bucket rawBucket = response->buckets[i];

			name = QString::fromUtf8(rawBucket.name->value);
			if(m_searchedModel->GetPath(index) != QString("/") && !m_searchedModel->GetPath(index).contains(name))
				continue;

			QString prefix = m_searchedModel->GetPath(index);
			// Because of GetPath(), the initial "/" needs to be removed for searches to work
			if(prefix.startsWith("/"))
				prefix.remove(0, 1);
			m_activeSearchCount++;
			Search(index, name, prefix, QString("%"+search+"%"));
		}
	}
}

void
DS3SearchModel::HandleGetObjectsResponse()
{
	// Temporary list to hold objects as they are found
	QList<ds3_search_object*> objectList;
	// Get the watcher and response
	GetObjectsWatcher* watcher = static_cast<GetObjectsWatcher*>(sender());
	ds3_get_objects_response* response = NULL;
	const QString& bucketName = watcher->GetBucketName();
	try {
		response = watcher->result();
	}
	catch (DS3Error& e) {
		QString msg;
		if (e.GetStatusCode() == 404) {
			msg = "Bucket \"" + bucketName + "\" does not exist";
		} else {
			msg = e.ToString();
		}
		LOG_ERROR("Error listing objects - " + msg);
	}
	// Checks that response isn't empty
	if(response != NULL) {
		for (size_t i = 0; i < response->num_objects; i++) {
			// Increment the found count and add the object to the search model
			objectList << response->objects[i];
		}
	}
	for(int i=0; i<objectList.size(); i++) {
		m_searchFoundCount++;
		m_foundList << objectList[i];
		m_bucketList << bucketName;
		// AppendItem(objectList[i], bucketName);
	}
	bool found = true;
	m_activeSearchCount--;

	if(m_activeSearchCount == 0) {
		// If no results were found, then create a fake item that tells
		//   this to the user
		if(m_searchFoundCount == 0) {
			found = false;
			ds3_search_object* empty;
			ds3_str* name = new ds3_str();
			char* nameChar = new char[40];
			strcpy(nameChar, "There are currently no items to display");
			name->value = nameChar;
			empty->name = name;
			m_foundList << empty;
			m_bucketList << QString("");
			// AppendItem(empty, QString(""));
		}
		AddToTree();
		emit DoneSearching(found);
	}

	delete watcher;
}

void
DS3SearchModel::AddToTree()
{
	for(int i=0; i<m_foundList.size(); i++) {
		AppendItem(m_foundList[i], m_bucketList[i]);
	}
}
