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

#include <QMap>

#include "lib/work_items/bulk_get_work_item.h"

BulkGetWorkItem::BulkGetWorkItem(const QString& host,
				 const QList<QUrl> urls,
				 const QString& destination)
	: BulkWorkItem(host, urls),
	  m_destination(destination)
{
	SortURLsByBucket();
}

// A DS3 bulk get request can only be for a single bucket, however,
// urls could contain objects from different buckets.
void
BulkGetWorkItem::SortURLsByBucket()
{
	QMap<QString, QUrl> sortMap;
	for (int i = 0; i < m_urls.size(); i++) {
		QUrl url = m_urls.at(i);
		sortMap.insert(url.toString(), url);
	}
	m_urls = sortMap.values();
	m_urlsIterator = m_urls.constBegin();
}
