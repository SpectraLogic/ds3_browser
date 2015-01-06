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

#ifndef BULK_WORK_ITEM_H
#define BULK_WORK_ITEM_H

#include <stdlib.h>
#include <QList>
#include <QString>
#include <QMutex>
#include <QUrl>

#include <ds3.h>

#include "lib/work_items/work_item.h"
#include "models/job.h"

class BulkWorkItem : public WorkItem
{
public:
	BulkWorkItem(const QString& host, const QList<QUrl> urls);
	virtual ~BulkWorkItem();

	// The S3 server assigned ID for this job.  This isn't valid until
	// a ds3_bulk_response response, with a valid job ID, has been set.
	const QString GetJobID() const;
	virtual Job::Type GetType() const = 0;
	Job::State GetState() const;
	const QString& GetHost() const;
	const QString& GetBucketName() const;
	const QList<QUrl> GetURLs() const;
	QList<QUrl>::const_iterator& GetUrlsIterator();
	const QList<QUrl>::const_iterator GetUrlsConstEnd() const;
	const QUrl& GetLastProcessedUrl() const;
	virtual const QString GetDestination() const = 0;
	uint64_t GetSize() const;
	uint64_t GetBytesTransferred() const;
	void UpdateBytesTransferred(size_t bytes);
	size_t GetNumChunksProcessed() const;

	bool WasCanceled() const;
	// A large drag/drop operation might have to be split up amonst
	// several DS3 Bulk GET/PUT jobs.  Each one of these DS3 jobs
	// represents a page.  IsPageFinished() signifies whether or not
	// the current DS3 Bulk GET/PUT "page" job is finished or not.
	// Note that each page could be split up amongst one or more
	// DS3 job chunks.
	bool IsPageFinished() const;
	// Signifies whether or not the entire work item as a whole is finished.
	// In other words, if an entire drag/drop operation is complete.
	bool IsFinished() const;

	void SetBucketName(const QString& bucketName);
	void SetLastProcessedUrl(const QUrl& url);
	void SetNumChunks(int chunks);
	void SetNumChunksProcessed(int chunks);
	void IncNumChunksProcessed(int chunks = 1);

	void ClearObjMap();
	QHash<QString, QString>::const_iterator GetObjMapConstBegin() const;
	QHash<QString, QString>::const_iterator GetObjMapConstEnd() const;
	uint64_t GetObjMapSize() const;
	const QString GetObjMapValue(const QString& objName) const;
	void InsertObjMap(const QString& objName, const QString& filePath);

	ds3_bulk_response* GetResponse() const;
	void SetResponse(ds3_bulk_response* response);

	void SetState(Job::State state);

	const Job ToJob() const;

protected:
	void SortURLsByBucket();

	Job::State m_state;
	mutable QMutex m_stateLock;
	QString m_host;
	QString m_bucketName;
	QList<QUrl> m_urls;
	QUrl m_lastProcessedUrl;
	QList<QUrl>::const_iterator m_urlsIterator;
	uint64_t m_bytesTransferred;
	mutable QMutex m_bytesTransferredLock;
	QHash<QString, QString> m_objMap;
	ds3_bulk_response* m_response;
	size_t m_numChunks;
	size_t m_numChunksProcessed;
	mutable QMutex m_numChunksLock;
};

inline const QString&
BulkWorkItem::GetHost() const
{
	return m_host;
}

inline const QString&
BulkWorkItem::GetBucketName() const
{
	return m_bucketName;
}

inline const QList<QUrl>
BulkWorkItem::GetURLs() const
{
	return m_urls;
}

inline QList<QUrl>::const_iterator&
BulkWorkItem::GetUrlsIterator()
{
	return m_urlsIterator;
}

inline const QList<QUrl>::const_iterator
BulkWorkItem::GetUrlsConstEnd() const
{
	return m_urls.constEnd();
}

inline const QUrl&
BulkWorkItem::GetLastProcessedUrl() const
{
	return m_lastProcessedUrl;
}

inline size_t
BulkWorkItem::GetNumChunksProcessed() const
{
	m_numChunksLock.lock();
	int chunks = m_numChunksProcessed;
	m_numChunksLock.unlock();
	return chunks;
}

inline bool
BulkWorkItem::WasCanceled() const
{
	Job::State state = GetState();
	return (state == Job::CANCELING || state == Job::CANCELED);
}


inline void
BulkWorkItem::SetBucketName(const QString& bucketName)
{
	m_bucketName = bucketName;
}

inline void
BulkWorkItem::SetLastProcessedUrl(const QUrl& url)
{
	m_lastProcessedUrl = url;
}

inline void
BulkWorkItem::SetNumChunks(int chunks)
{
	m_numChunksLock.lock();
	m_numChunks = chunks;
	m_numChunksLock.unlock();
}

inline void
BulkWorkItem::SetNumChunksProcessed(int chunks)
{
	m_numChunksLock.lock();
	m_numChunksProcessed = chunks;
	m_numChunksLock.unlock();
}

inline void
BulkWorkItem::IncNumChunksProcessed(int chunks)
{
	m_numChunksLock.lock();
	m_numChunksProcessed += chunks;
	m_numChunksLock.unlock();
}

inline void
BulkWorkItem::ClearObjMap()
{
	m_objMap.clear();
}

inline QHash<QString,QString>::const_iterator
BulkWorkItem::GetObjMapConstBegin() const
{
	return m_objMap.constBegin();
}

inline QHash<QString,QString>::const_iterator
BulkWorkItem::GetObjMapConstEnd() const
{
	return m_objMap.constEnd();
}

inline uint64_t
BulkWorkItem::GetObjMapSize() const
{
	return (uint64_t)m_objMap.size();
}

inline const QString
BulkWorkItem::GetObjMapValue(const QString& objName) const
{
	return m_objMap[objName];
}

inline void
BulkWorkItem::InsertObjMap(const QString& objName, const QString& filePath)
{
	m_objMap.insert(objName, filePath);
}

inline Job::State
BulkWorkItem::GetState() const
{
	m_stateLock.lock();
	Job::State state = m_state;
	m_stateLock.unlock();
	return state;
}

inline ds3_bulk_response*
BulkWorkItem::GetResponse() const
{
	return m_response;
}

inline void
BulkWorkItem::SetResponse(ds3_bulk_response* response)
{
	m_response = response;
}

inline void
BulkWorkItem::SetState(Job::State state)
{
	m_stateLock.lock();
	m_state = state;
	m_stateLock.unlock();
}

#endif
