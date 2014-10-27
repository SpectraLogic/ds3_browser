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

#include "lib/client.h"
#include "models/bucket.h"

// Must match Bucket::Column
const char* const Bucket::COLUMN_NAMES[] = { "Name",
					     "Owner",
					     "Created" };

Bucket::Bucket(Client* client, QObject* parent)
	: QAbstractItemModel(parent),
	  m_client(client),
	  m_get_service_response(NULL)
{
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
	int count = 0;

	if (!parent.isValid())
	{
		m_get_service_response_lock.lock();
		count = GetGetServiceResponse()->num_buckets;
		m_get_service_response_lock.unlock();
	}

	return count;
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
	QVariant data;
	ds3_get_service_response* get_service_response;

	if (index.parent().isValid()) {
		// TODO fetch and display objects
		return data;
	}

	switch (role)
	{
	case Qt::DisplayRole:
		m_get_service_response_lock.lock();
		get_service_response = GetGetServiceResponse();
		switch (column)
		{
		case NAME:
			name = get_service_response->buckets[index.row()].name->value;
			data = QString(QLatin1String(name));
			break;
		case OWNER:
			owner = get_service_response->owner->name->value;
			data = QString(QLatin1String(owner));
			break;
		case CREATED:
			createdStr = get_service_response->buckets[index.row()].creation_date->value;
			created = QDateTime::fromString(QString(QLatin1String(createdStr)),
							"yyyy-MM-ddThh:mm:ss.000Z");
			data = created.toString("MMMM d, yyyy h:mm AP");
			break;
		}
		m_get_service_response_lock.unlock();
		break;
	case Qt::DecorationRole:
		if (column == NAME) {
			data = QIcon(":/resources/icons/bucket.png");
		}
		break;
	}
	return data;
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

void
Bucket::Refresh()
{
	beginResetModel();
	m_get_service_response_lock.lock();
	m_get_service_response = NULL;
	m_get_service_response_lock.unlock();
	endResetModel();
}

ds3_get_service_response*
Bucket::GetGetServiceResponse() const
{
	if (m_get_service_response != NULL) {
		return m_get_service_response;
	}

	m_get_service_response = m_client->GetService();
	return m_get_service_response;
}
