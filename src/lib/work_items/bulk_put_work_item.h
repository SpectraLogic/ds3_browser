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

#ifndef BULK_PUT_WORK_ITEM_H
#define BULK_PUT_WORK_ITEM_H

#include <QDirIterator>
#include <QHash>
#include <QList>
#include <QMutex>
#include <QString>
#include <QUrl>

#include <ds3.h>

// BulkPutWorkItem, a container class that stores all data necessary to perform
// a DS3 bulk put operation.
class BulkPutWorkItem
{
public:
	BulkPutWorkItem(const QString& bucketName,
			const QString& prefix,
			const QList<QUrl> urls);
	~BulkPutWorkItem();

	const QString& GetBucketName() const;
	const QString& GetPrefix() const;
	QList<QUrl>::const_iterator& GetUrlsIterator();
	const QList<QUrl>::const_iterator GetUrlsConstEnd() const;

	QDirIterator* GetDirIterator() const;
	QDirIterator* GetDirIterator(const QString& filePath);
	void DeleteDirIterator();

	void ClearObjMap();
	QHash<QString, QString>::const_iterator GetObjMapConstBegin() const;
	QHash<QString, QString>::const_iterator GetObjMapConstEnd() const;
	int GetObjMapSize() const;
	const QString GetObjMapValue(const QString& objName) const;
	void InsertObjMap(const QString& objName, const QString& filePath);

	void IncWorkingObjListCount();
	void DecWorkingObjListCount();
	int GetWorkingObjListCount() const;

	bool IsPageFinished() const;
	bool IsFinished() const;

	ds3_bulk_response* GetResponse() const;
	void SetResponse(ds3_bulk_response* response);

private:
	QString m_bucketName;
	QString m_prefix;
	QList<QUrl> m_urls;
	QList<QUrl>::const_iterator m_urlsIterator;
	QDirIterator* m_dirIterator;
	QHash<QString, QString> m_objMap;
	ds3_bulk_response* m_response;

	// The number of active object list threads.  This is used to determine
	// if the Client is finished with this work item's bulkput page.
	int m_workingObjListCount;
	mutable QMutex m_workingObjListCountLock;
};

inline const QString&
BulkPutWorkItem::GetBucketName() const
{
	return m_bucketName;
}

inline const QString&
BulkPutWorkItem::GetPrefix() const
{
	return m_prefix;
}

inline QList<QUrl>::const_iterator&
BulkPutWorkItem::GetUrlsIterator()
{
	return m_urlsIterator;
}

inline const QList<QUrl>::const_iterator
BulkPutWorkItem::GetUrlsConstEnd() const
{
	return m_urls.constEnd();
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

inline void
BulkPutWorkItem::ClearObjMap()
{
	m_objMap.clear();
}

inline QHash<QString,QString>::const_iterator
BulkPutWorkItem::GetObjMapConstBegin() const
{
	return m_objMap.constBegin();
}

inline QHash<QString,QString>::const_iterator
BulkPutWorkItem::GetObjMapConstEnd() const
{
	return m_objMap.constEnd();
}

inline int
BulkPutWorkItem::GetObjMapSize() const
{
	return m_objMap.size();
}

inline const QString
BulkPutWorkItem::GetObjMapValue(const QString& objName) const
{
	return m_objMap[objName];
}

inline void
BulkPutWorkItem::InsertObjMap(const QString& objName, const QString& filePath)
{
	m_objMap.insert(objName, filePath);
}

inline void
BulkPutWorkItem::IncWorkingObjListCount()
{
	m_workingObjListCountLock.lock();
	m_workingObjListCount++;
	m_workingObjListCountLock.unlock();
}

inline void
BulkPutWorkItem::DecWorkingObjListCount()
{
	m_workingObjListCountLock.lock();
	m_workingObjListCount--;
	m_workingObjListCountLock.unlock();
}

inline int
BulkPutWorkItem::GetWorkingObjListCount() const
{
	return m_workingObjListCount;
}

inline ds3_bulk_response*
BulkPutWorkItem::GetResponse() const
{
	return m_response;
}

inline void
BulkPutWorkItem::SetResponse(ds3_bulk_response* response)
{
	m_response = response;
}

#endif
