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

#ifndef OBJECT_WORK_ITEM_H
#define OBJECT_WORK_ITEM_H

#include <QFile>
#include <QIODevice>
#include <QString>

#include "lib/work_items/work_item.h"

class BulkWorkItem;

// ObjectWorkItem, a container class used to pass information about a
// GET/PUT object request to the methods that actually read/write the
// files.  This allows those file read/write callback functions to do things
// like send out progress updates.
class ObjectWorkItem : public WorkItem
{
public:
	ObjectWorkItem(const QString& bucketName,
		       const QString& objectName,
		       const QString& fileName,
		       BulkWorkItem* bulkWorkItem = NULL);
	~ObjectWorkItem();

	const QString& GetBucketName() const;
	const QString& GetObjectName() const;
	QFile* GetFile();
	BulkWorkItem* GetBulkWorkItem() const;

	bool OpenFile(QIODevice::OpenMode mode);
	bool SeekFile(uint64_t pos);
	size_t ReadFile(char* data, size_t size, size_t count);
	size_t WriteFile(char* data, size_t size, size_t count);

private:
	QString m_bucketName;
	QString m_objectName;
	QFile m_file;
	BulkWorkItem* m_bulkWorkItem;
};

inline const QString&
ObjectWorkItem::GetBucketName() const
{
	return m_bucketName;
}

inline const QString&
ObjectWorkItem::GetObjectName() const
{
	return m_objectName;
}

inline QFile*
ObjectWorkItem::GetFile()
{
	return &m_file;
}

inline BulkWorkItem*
ObjectWorkItem::GetBulkWorkItem() const
{
	return m_bulkWorkItem;
}

inline bool
ObjectWorkItem::OpenFile(QIODevice::OpenMode mode)
{
	return m_file.open(mode);
}

inline bool
ObjectWorkItem::SeekFile(uint64_t pos)
{
	return m_file.seek(pos);
}

#endif
