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

#ifndef GET_SERVICE_WATCHER_H
#define GET_SERVICE_WATCHER_H

#include <QFuture>
#include <QFutureWatcher>
#include <QModelIndex>

#include <ds3.h>

class GetServiceWatcher : public QFutureWatcher<ds3_get_service_response*>
{
public:
	GetServiceWatcher(const QModelIndex& parentModelIndex,
			  QObject* parent = 0);

	const QModelIndex& GetParentModelIndex() const;

private:
	const QModelIndex m_parentModelIndex;
};

inline const QModelIndex&
GetServiceWatcher::GetParentModelIndex() const
{
	return m_parentModelIndex;
}

#endif
