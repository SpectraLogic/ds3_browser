#include <QDateTime>
#include <QIcon>

#include "bucket.h"

// Must match Bucket::Column
const char* const Bucket::COLUMN_NAMES[] = { "Name",
					     "Owner",
					     "Created" };

Bucket::Bucket(ds3_get_service_response* response, QObject* parent)
	: QAbstractItemModel(parent),
	  m_get_service_response(response)	
{
}

Bucket::~Bucket()
{
	ds3_free_service_response(m_get_service_response);
}

QModelIndex
Bucket::index(int row, int column, const QModelIndex &parent) const
{
	return hasIndex(row, column, parent) ? createIndex(row, column) : QModelIndex();
}

// rowCount actually determines whether or not the bucket has any objects in
// it.  hasChildren always returns true so the expand/collapse caret is
// always displayed even if we don't yet know if the bucket has any objects.
bool
Bucket::hasChildren(const QModelIndex &/*parent*/) const
{
	return true;
}

int
Bucket::columnCount(const QModelIndex &/*parent*/) const
{
	return COUNT;
}

int
Bucket::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
	{
		return 0;
	}
	return m_get_service_response->num_buckets;
}

QModelIndex
Bucket::parent(const QModelIndex &/*index*/) const
{
	QModelIndex invalidQModelIndex;
	return invalidQModelIndex;
}

QVariant
Bucket::data(const QModelIndex &index, int role) const
{
	char* name;
	char* owner;
	char* createdStr;
	QDateTime created;
	int column = index.column();

	switch (role)
	{
	case Qt::DisplayRole:
		switch (column)
		{
		case NAME:
			name = m_get_service_response->buckets[index.row()].name->value;
			return QString(QLatin1String(name));
		case OWNER:
			owner = m_get_service_response->owner->name->value;
			return QString(QLatin1String(owner));
		case CREATED:
			createdStr = m_get_service_response->buckets[index.row()].creation_date->value;
			created = QDateTime::fromString(QString(QLatin1String(createdStr)),
							"yyyy-MM-ddThh:mm:ss.000Z");
			return created.toString("MMMM d, yyyy h:mm AP");
		}
	case Qt::DecorationRole:
		if (column == NAME) {
			return QIcon(":/resources/icons/bucket.png");
		}
	}
	return QVariant();
}

QVariant
Bucket::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
	if (role == Qt::DisplayRole)
	{
		return COLUMN_NAMES[section];
	}
	return QVariant();
}
