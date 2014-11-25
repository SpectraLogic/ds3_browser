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

#include "lib/work_items/bulk_work_item.h"
#include "models/job.h"

BulkWorkItem::BulkWorkItem(const QString& host, const QString& bucketName)
	: WorkItem(),
	  m_state(Job::INITIALIZING),
	  m_host(host),
	  m_bucketName(bucketName),
	  m_bytesTransferred(0),
	  m_response(NULL)
{
}

BulkWorkItem::~BulkWorkItem()
{
}

uint64_t
BulkWorkItem::GetBytesTransferred() const
{
	uint64_t bytesTransferred = 0;
	m_bytesTransferredLock.lock();
	bytesTransferred = m_bytesTransferred;
	m_bytesTransferredLock.unlock();
	return bytesTransferred;
}

void
BulkWorkItem::UpdateBytesTransferred(size_t bytes)
{
	m_bytesTransferredLock.lock();
	m_bytesTransferred += bytes;
	m_bytesTransferredLock.unlock();
}

const QString
BulkWorkItem::GetJobID() const
{
	QString jobID;
	if (m_response != NULL) {
		jobID = QString(m_response->job_id->value);
	}
	return jobID;
}

uint64_t
BulkWorkItem::GetSize() const
{
	uint64_t size = 0;
	if (m_response != NULL) {
		size = m_response->original_size_in_bytes;
	}
	return size;
}

const Job
BulkWorkItem::ToJob() const
{
	Job job;
	job.SetID(GetID());
	job.SetType(GetType());
	job.SetStart(GetStart());
	job.SetState(GetState());
	job.SetHost(GetHost());
	job.SetBucketName(GetBucketName());
	job.SetSize(GetSize());
	job.SetBytesTransferred(GetBytesTransferred());
	return job;
}
