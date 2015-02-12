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

#include <QMap>

#include "lib/work_items/bulk_work_item.h"
#include "models/job.h"

BulkWorkItem::BulkWorkItem(const QString& host, const QList<QUrl> urls)
	: WorkItem(),
	  m_state(Job::INITIALIZING),
	  m_host(host),
	  m_urls(urls),
	  m_urlsIterator(m_urls.constBegin()),
	  m_bytesTransferred(0),
	  m_response(NULL),
	  m_numChunksProcessed(0)
{
	SortURLsByBucket();
}

BulkWorkItem::~BulkWorkItem()
{
	if (m_response != NULL) {
		ds3_free_bulk_response(m_response);
	}
}

uint64_t
BulkWorkItem::GetBytesTransferred() const
{
	m_bytesTransferredLock.lock();
	uint64_t bytesTransferred = m_bytesTransferred;
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

bool
BulkWorkItem::IsPageFinished() const
{
	bool finished = true;
	m_responseLock.lock();
	if (m_response != NULL) {
		size_t numChunks = m_response->list_size;
		finished = numChunks == m_numChunksProcessed;
	}
	m_responseLock.unlock();
	return finished;
}

bool
BulkWorkItem::IsFinished() const
{
	return (IsPageFinished() && m_urlsIterator == GetUrlsConstEnd());
}

const Job
BulkWorkItem::ToJob() const
{
	Job job;
	job.SetID(GetID());
	job.SetType(GetType());
	job.SetStart(GetStart());
	job.SetTransferStart(GetTransferStart());
	job.SetState(GetState());
	job.SetHost(GetHost());
	job.SetURLs(GetURLs());
	job.SetDestination(GetDestination());
	job.SetSize(GetSize());
	job.SetBytesTransferred(GetBytesTransferred());
	return job;
}


// Sort the URLs alphabetically so it's easier to determine if one URL is a
// descendant of another.  Plus, a DS3 bulk get request can only be for a
// single bucket, however, urls could contain objects from different buckets.
void
BulkWorkItem::SortURLsByBucket()
{
	QMap<QString, QUrl> sortMap;
	for (int i = 0; i < m_urls.size(); i++) {
		QUrl url = m_urls.at(i);
		sortMap.insert(url.toString(), url);
	}
	m_urls = sortMap.values();
	m_urlsIterator = m_urls.constBegin();
}
