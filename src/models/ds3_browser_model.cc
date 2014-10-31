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

#include <QDateTime>
#include <QIcon>
#include <QSet>

#include "lib/client.h"
#include "models/ds3_browser_model.h"

// Must match m_rootItem->m_data;
enum Column { NAME, OWNER, SIZE, KIND, CREATED, COUNT };

static const QString REST_TIMESTAMP_FORMAT = "yyyy-MM-ddThh:mm:ss.000Z";
static const QString VIEW_TIMESTAMP_FORMAT = "MMMM d, yyyy h:mm AP";

static const QString BUCKET = "Bucket";
static const QString OBJECT = "Object";
static const QString FOLDER = "Folder";
static const QString BREAK = "Break";

//
// DS3BrowserItem
//

class DS3BrowserItem
{
public:
	DS3BrowserItem(Client* client,
		       const QList<QVariant>& data,
		       QString bucketName = QString(),
		       QString prefix = QString(),
		       DS3BrowserItem* parent = 0);
	~DS3BrowserItem();

	DS3BrowserItem* GetChild(int row);
	void RemoveChild(int row);
	int GetChildCount();
	int GetColumnCount() const;
	QVariant GetData(int column) const;
	int GetRow() const;
	DS3BrowserItem* GetParent();
	void Reset();
	QString GetPath();

	void FetchObjects();

private:
	Client* m_client;
	QList<DS3BrowserItem*> m_children;
	// So m_children can be lazily populated
	bool m_childrenInitialized;
	// List of data to show in the table.  Each item in the list
	// directly corresponds to a column.
	QList<QVariant> m_data;
	// So object items so they can easily keep track of what
	// bucket they're in.  For bucket items, this is the same as
	// m_data[0];
	const QString m_bucketName;
	// All parent folder object names, not including the bucket name
	const QString m_prefix;
	DS3BrowserItem* m_parent;

	bool m_childrenTruncated;
	QString m_childrenNextMarker;
	uint32_t m_childrenMaxKeys;

	QList<DS3BrowserItem*> GetChildren();
	void FetchChildren();
	void FetchBuckets();
};

DS3BrowserItem::DS3BrowserItem(Client* client,
			       const QList<QVariant>& data,
			       QString bucketName,
			       QString prefix,
			       DS3BrowserItem* parent)
	: m_client(client),
	  m_childrenInitialized(false),
	  m_data(data),
	  m_bucketName(bucketName),
	  m_prefix(prefix),
	  m_parent(parent),
	  m_childrenTruncated(false),
	  m_childrenMaxKeys(1000)
{
}

DS3BrowserItem::~DS3BrowserItem()
{
	qDeleteAll(m_children);
}

DS3BrowserItem*
DS3BrowserItem::GetChild(int row)
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
DS3BrowserItem::GetChildCount()
{
	return GetChildren().count();	
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
	return m_data.value(column);
}

inline DS3BrowserItem*
DS3BrowserItem::GetParent()
{
	return m_parent;
}

void
DS3BrowserItem::Reset()
{
	FetchChildren();
}

QString
DS3BrowserItem::GetPath()
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
DS3BrowserItem::GetChildren()
{
	if (!m_childrenInitialized) {
		FetchChildren();
		m_childrenInitialized = true;
	}
	return m_children;
}

void
DS3BrowserItem::FetchChildren()
{
	qDeleteAll(m_children);
	m_children.clear();

	m_parent ? FetchObjects() : FetchBuckets();
}

void
DS3BrowserItem::FetchBuckets()
{
	ds3_get_service_response* response = m_client->GetService();
	QString owner = QString(QLatin1String(response->owner->name->value));

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

		name = QString(QLatin1String(rawBucket.name->value));
		bucketData << name;

		bucketData << owner;

		bucketData << "--";

		bucketData << BUCKET;

		rawCreated = rawBucket.creation_date->value;
		createdDT = QDateTime::fromString(QString(QLatin1String(rawCreated)),
				    		  REST_TIMESTAMP_FORMAT);
		created = createdDT.toString(VIEW_TIMESTAMP_FORMAT);
		bucketData << created;

		bucket = new DS3BrowserItem(m_client,
					    bucketData,
					    name,
					    QString(),
					    this);
		m_children << bucket;
	}

	ds3_free_service_response(response);
}

void
DS3BrowserItem::FetchObjects()
{
	bool isBucket = GetData(KIND) == BUCKET;
	QString name = GetData(NAME).toString();
	std::string bucketName = m_bucketName.toUtf8().constData();
	QString prefix = m_prefix;
	if (!isBucket) {
		prefix += name + "/";
	}

	std::string nextMarker;
	uint32_t maxKeys = m_childrenMaxKeys;
	if (m_childrenTruncated) {
		nextMarker = m_childrenNextMarker.toUtf8().constData();
	}
	ds3_get_bucket_response* response = m_client->GetBucket(bucketName,
								prefix.toUtf8().constData(),
								"/",
								nextMarker,
								maxKeys);

	QVariant owner = GetData(OWNER);

	QSet<QString> currentCommonPrefixNames;
	for (int i = 0; i < m_children.count(); i++) {
		DS3BrowserItem* object = m_children.at(i);
		if (object->GetData(KIND) == FOLDER) {
			currentCommonPrefixNames << object->GetData(NAME).toString();
		}
	}

	for (size_t i = 0; i < response->num_common_prefixes; i++) {
		ds3_str* rawCommonPrefix = response->common_prefixes[i];
		// The order in which bucketData is filled must match
		// Column
		QList<QVariant> objectData;
		DS3BrowserItem* object;

		QString nextName = QString(QLatin1String(rawCommonPrefix->value));
		nextName.replace(QRegExp("^" + prefix), "");
		nextName.replace(QRegExp("/$"), "");
		if (!currentCommonPrefixNames.contains(nextName)) {
			objectData << nextName;
			objectData << owner;
			objectData << "--";
			objectData << FOLDER;
			objectData << "--";
			object = new DS3BrowserItem(m_client,
						    objectData,
						    m_bucketName,
						    prefix,
						    this);
			m_children << object;
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

		nextName = QString(QLatin1String(rawObject.name->value));
		nextName.replace(QRegExp("^" + prefix), "");
		objectData << nextName;

		objectData << owner;

		// TODO Humanize the size
		objectData << rawObject.size;

		objectData << OBJECT;

		rawCreated = rawObject.last_modified->value;
		createdDT = QDateTime::fromString(QString(QLatin1String(rawCreated)),
				    		  REST_TIMESTAMP_FORMAT);
		created = createdDT.toString(VIEW_TIMESTAMP_FORMAT);
		objectData << created;

		object = new DS3BrowserItem(m_client,
					    objectData,
					    m_bucketName,
					    prefix,   	
					    this);
		m_children << object;
	}

	m_childrenTruncated = response->is_truncated;
	if (response->next_marker) {
		m_childrenNextMarker = QString(QLatin1String(response->next_marker->value));
	}
	m_childrenMaxKeys = response->max_keys;

	if (m_childrenTruncated) {
		QList<QVariant> pageBreakData;
		pageBreakData << "Click to load more" << "" << "" << BREAK;
		DS3BrowserItem* pageBreak = new DS3BrowserItem(m_client,
							       pageBreakData,
							       m_bucketName,
							       prefix,
							       this);
		m_children << pageBreak;
	}

	ds3_free_bucket_response(response);
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
	m_rootItem = new DS3BrowserItem(m_client, column_names);
}

DS3BrowserModel::~DS3BrowserModel()
{
	delete m_rootItem;
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
	if (kind == BUCKET || kind == FOLDER) {
		return true;
	} else {
		return false;
	}
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
		if (column == 0 && item->GetData(KIND) == BREAK) {
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

QVariant
DS3BrowserModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
	if (role == Qt::DisplayRole) {
		return m_rootItem->GetData(section);
	}
	return QVariant();
}

bool
DS3BrowserModel::IsBucketOrFolder(const QModelIndex& index) const
{
	DS3BrowserItem* item = IndexToItem(index);
	QVariant kind = item->GetData(KIND);
	return (kind == BUCKET || kind == FOLDER);
}

bool
DS3BrowserModel::IsBreak(const QModelIndex& index) const
{
	DS3BrowserItem* item = IndexToItem(index);
	QVariant kind = item->GetData(KIND);
	return (kind == BREAK);
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
DS3BrowserModel::Refresh()
{
	beginResetModel();
	m_rootItem->Reset();
	endResetModel();
}

void
DS3BrowserModel::FetchNextPage(const QModelIndex& pageBreakIndex)
{
	QModelIndex parentIndex = pageBreakIndex.parent();
	int pageBreakRow = pageBreakIndex.row();
	DS3BrowserItem* parent = IndexToItem(parentIndex);
	// TODO Make use of beginInsertRows and endInsertRows, which might
	//      require moving the FetchObjects out of DS3BrowserItem and into
	//      this class.
	parent->FetchObjects();
	removeRow(pageBreakRow, parentIndex);
}

bool
DS3BrowserModel::removeRows(int row, int count, const QModelIndex& parentIndex)
{
	if (row < 0 || count <= 0 || (row + count) > rowCount(parentIndex)) {
		return false;
	}

	DS3BrowserItem* parent = IndexToItem(parentIndex);

	beginRemoveRows(parentIndex, row, row + count - 1);

	for (int i = 0; i < count; i++) {
		parent->RemoveChild(row + i);
	}

	endRemoveRows();

	return true;
}
