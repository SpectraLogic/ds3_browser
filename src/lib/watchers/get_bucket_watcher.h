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

#ifndef GET_BUCKET_WATCHER_H
#define GET_BUCKET_WATCHER_H

#include <QFuture>
#include <QFutureWatcher>
#include <QModelIndex>
#include <QString>

#include <ds3.h>

class GetBucketWatcher : public QFutureWatcher<ds3_get_bucket_response*>
{
public:
	GetBucketWatcher(const QModelIndex& parentModelIndex,
			 const QString& bucketName,
			 const QString& pefix,
			 QObject* parent = 0);

	const QModelIndex& GetParentModelIndex() const;
	const QString& GetBucketName() const;
	const QString& GetPrefix() const;

private:
	const QModelIndex m_parentModelIndex;
	const QString m_bucketName;
	const QString m_prefix;
};

inline const QModelIndex&
GetBucketWatcher::GetParentModelIndex() const
{
	return m_parentModelIndex;
}

inline const QString&
GetBucketWatcher::GetBucketName() const
{
	return m_bucketName;
}

inline const QString&
GetBucketWatcher::GetPrefix() const
{
	return m_prefix;
}

#endif
