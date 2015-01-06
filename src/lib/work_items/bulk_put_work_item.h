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

#ifndef BULK_PUT_WORK_ITEM_H
#define BULK_PUT_WORK_ITEM_H

#include <QDir>
#include <QDirIterator>
#include <QHash>
#include <QList>
#include <QMutex>
#include <QString>
#include <QUrl>

#include "lib/work_items/bulk_work_item.h"

// BulkPutWorkItem, a container class that stores all data necessary to perform
// a DS3 bulk put operation.
class BulkPutWorkItem : public BulkWorkItem
{
public:
	BulkPutWorkItem(const QString& host,
			const QList<QUrl> urls,
			const QString& bucketName,
			const QString& prefix);
	~BulkPutWorkItem();

	Job::Type GetType() const;

	const QString GetDestination() const;
	const QString& GetPrefix() const;

	QDirIterator* GetDirIterator() const;
	QDirIterator* GetDirIterator(const QString& filePath);
	void DeleteDirIterator();

	bool IsFinished() const;

private:
	QString m_prefix;
	QDirIterator* m_dirIterator;
};

inline Job::Type
BulkPutWorkItem::GetType() const
{
	return Job::PUT;
}

inline const QString
BulkPutWorkItem::GetDestination() const
{
	return QDir::cleanPath(m_bucketName + "/" + m_prefix);
}

inline const QString&
BulkPutWorkItem::GetPrefix() const
{
	return m_prefix;
}

inline QDirIterator*
BulkPutWorkItem::GetDirIterator() const
{
	return m_dirIterator;
}

inline QDirIterator*
BulkPutWorkItem::GetDirIterator(const QString& filePath)
{
	DeleteDirIterator();
	m_dirIterator = new QDirIterator(filePath,
					 QDir::AllDirs | QDir::Files |
					 QDir::Hidden | QDir::Readable |
					 QDir::System | QDir::NoDotAndDotDot,
					 QDirIterator::Subdirectories);
	return m_dirIterator;
}

#endif
