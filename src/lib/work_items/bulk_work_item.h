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

#ifndef BULK_WORK_ITEM_H
#define BULK_WORK_ITEM_H

#include <stdlib.h>
#include <QString>
#include <QMutex>

#include <ds3.h>

#include "lib/work_items/work_item.h"
#include "models/job.h"

class BulkWorkItem : public WorkItem
{
public:
	BulkWorkItem(const QString& host, const QString& bucketName);

	// The S3 server assigned ID for this job.  This isn't valid until
	// a ds3_bulk_response response, with a valid job ID, has been set.
	const QString GetJobID() const;
	Job::State GetState() const;
	const QString& GetHost() const;
	const QString& GetBucketName() const;
	uint64_t GetSize() const;
	uint64_t GetBytesTransferred() const;
	void UpdateBytesTransferred(size_t bytes);

	ds3_bulk_response* GetResponse() const;
	void SetResponse(ds3_bulk_response* response);

	void SetState(Job::State state);

	const Job ToJob() const;

protected:
	Job::State m_state;
	QString m_host;
	QString m_bucketName;
	uint64_t m_bytesTransferred;
	mutable QMutex m_bytesTransferredLock;
	ds3_bulk_response* m_response;
};

inline const QString&
BulkWorkItem::GetHost() const
{
	return m_host;
}

inline Job::State
BulkWorkItem::GetState() const
{
	return m_state;
}

inline const QString&
BulkWorkItem::GetBucketName() const
{
	return m_bucketName;
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
	m_state = state;
}

#endif
