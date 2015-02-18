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

#ifndef CLIENT_H
#define CLIENT_H

#include <QFuture>
#include <QHash>
#include <QList>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QUuid>
#include <QUrl>

#include <ds3.h>

#include "lib/errors/ds3_error.h"
#include "models/job.h"

class BulkWorkItem;
class BulkGetWorkItem;
class BulkPutWorkItem;
class ObjectWorkItem;
class Session;

class Client : public QObject
{
	Q_OBJECT

public:
	static const QString DELIMITER;
	static const uint64_t BULK_PAGE_LIMIT;
	static const uint32_t MAX_KEYS;

	Client(const Session* session);
	~Client();

	QString GetEndpoint() const;

	int GetNumActiveJobs() const;
	void CancelActiveJobs();

	QFuture<ds3_get_service_response*> GetService();
	QFuture<ds3_get_bucket_response*> GetBucket(const QString& bucketName,
						    const QString& prefix,
						    const QString& marker,
						    bool silent = false);

	void CreateBucket(const QString& name);
	void DeleteBucket(const QString& name);
	void DeleteObject(const QString& bucketName, const QString& objectName);

	void BulkGet(const QList<QUrl> urls, const QString& destination);

	void BulkPut(const QString& bucketName,
		     const QString& prefix,
		     const QList<QUrl> urls);

	void GetObject(const QString& bucket,
		       const QString& object,
		       const QString& fileName,
		       uint64_t offset,
		       BulkGetWorkItem* bulkGetWorkItem);

	void PutObject(const QString& bucket,
		       const QString& object,
		       const QString& fileName,
		       uint64_t offset,
		       uint64_t length,
		       BulkPutWorkItem* bulkPutWorkItem);

public slots:
	// Cancel an in-progress BulkGet or BulkPut request.
	void CancelBulkJob(QUuid workItemID);

signals:
	void JobProgressUpdate(const Job job);

private:
	ds3_get_service_response* DoGetService();
	ds3_get_bucket_response* DoGetBucket(const QString& bucketName,
					     const QString& prefix,
					     const QString& delimiter,
					     const QString& marker,
					     bool silent = false);

	void PrepareBulkGets(BulkGetWorkItem* workItem);
	void PrepareBulkPuts(BulkPutWorkItem* workItem);
	void DoBulk(BulkWorkItem* workItem);

	void CreateBulkGetDirs(BulkGetWorkItem* workItem);
	void ProcessGetJobChunk(BulkGetWorkItem* workItem);
	ds3_get_available_chunks_response* GetAvailableJobChunks(BulkGetWorkItem* bulkGetWorkItem);

	void ProcessPutJobChunk(BulkPutWorkItem* workItem);
	ds3_allocate_chunk_response* AllocateJobChunk(const char* chunkID);

	void DeleteOrRequeueBulkWorkItem(BulkWorkItem* workItem);
	void DeleteBulkWorkItem(BulkWorkItem* workItem);

	QString m_host;
	QString m_endpoint;
	ds3_creds* m_creds;
	ds3_client* m_client;
	QHash<QUuid, BulkWorkItem*> m_bulkWorkItems;
	mutable QMutex m_bulkWorkItemsLock;

public:
	// Meant to be private but called from the C SDK callback function
	size_t ReadFile(ObjectWorkItem* workItem, char* buffer,
			size_t size, size_t count);
	// Meant to be private but called from the C SDK callback function
	size_t WriteFile(ObjectWorkItem* workItem, char* buffer,
			 size_t size, size_t count);

};

inline QString
Client::GetEndpoint() const
{
	return m_endpoint;
}

#endif
