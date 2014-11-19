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

#include "lib/work_items/bulk_put_work_item.h"

BulkPutWorkItem::BulkPutWorkItem(const QString& bucketName,
				 const QString& prefix,
				 const QList<QUrl> urls)
	: m_bucketName(bucketName),
	  m_prefix(prefix),
	  m_urls(urls),
	  m_urlsIterator(m_urls.constBegin()),
	  m_dirIterator(NULL),
	  m_response(NULL),
	  m_workingObjListCount(0)
{
}

BulkPutWorkItem::~BulkPutWorkItem()
{
	DeleteDirIterator();
	if (m_response != NULL) {
		ds3_free_bulk_response(m_response);
	}
}

void
BulkPutWorkItem::DeleteDirIterator()
{
	if (m_dirIterator != NULL) {
		delete m_dirIterator;
		m_dirIterator = NULL;
	}
}

bool
BulkPutWorkItem::IsPageFinished() const
{
	m_workingObjListCountLock.lock();
	bool finished = m_workingObjListCount == 0;
	m_workingObjListCountLock.unlock();
	return finished;
}

bool
BulkPutWorkItem::IsFinished() const
{
	bool finished = IsPageFinished() &&
			m_urlsIterator == GetUrlsConstEnd() &&
			m_dirIterator == NULL;
	return finished;
}
