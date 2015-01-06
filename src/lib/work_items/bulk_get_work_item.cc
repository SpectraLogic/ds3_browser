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

#include "lib/work_items/bulk_get_work_item.h"

BulkGetWorkItem::BulkGetWorkItem(const QString& host,
				 const QList<QUrl> urls,
				 const QString& destination)
	: BulkWorkItem(host, urls),
	  m_destination(destination),
	  m_getBucketResponse(NULL),
	  m_getBucketResponseIterator(0)
{
}
