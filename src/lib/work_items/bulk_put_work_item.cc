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

BulkPutWorkItem::BulkPutWorkItem(const QString& host,
				 const QList<QUrl> urls,
				 const QString& bucketName,
				 const QString& prefix)
	: BulkWorkItem(host, urls),
	  m_prefix(prefix),
	  m_dirIterator(NULL)
{
	  m_bucketName = bucketName;
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
BulkPutWorkItem::IsFinished() const
{
	return (BulkWorkItem::IsFinished() && m_dirIterator == NULL);
}
