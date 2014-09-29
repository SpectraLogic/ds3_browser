#include <QIcon>
#include <QIcon>

#include "bucket.h"

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
	return 1;
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

	switch (role)
	{
	case Qt::DisplayRole:
		name = m_get_service_response->buckets[index.row()].name->value;
		return QString(QLatin1String(name));
	case Qt::DecorationRole:
		return QIcon(":/resources/icons/bucket.png");
	}
	return QVariant();
}

QVariant
Bucket::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
	if (role == Qt::DisplayRole)
	{
		switch (section)
		{
		case 0:
			return "Name";
		}
	}
	return QVariant();
}
