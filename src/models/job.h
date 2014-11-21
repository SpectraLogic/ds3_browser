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

#ifndef JOB_H
#define JOB_H

#include <stdint.h>
#include <QMetaType>
#include <QString>

class Job
{
public:
	Job();

	const QString& GetID() const;
	const QString& GetHost() const;
	const QString& GetBucketName() const;
	uint64_t GetSize() const;
	uint64_t GetBytesTransferred() const;

	void SetID(const QString& id);
	void SetHost(const QString& host);
	void SetBucketName(const QString& bucketName);
	void SetSize(uint64_t);
	void SetBytesTransferred(uint64_t);

private:
	QString m_id;
	QString m_host;
	QString m_bucketName;
	uint64_t m_size;
	uint64_t m_bytesTransferred;
};

// Job is used as an argument in a signal/slot connection
Q_DECLARE_METATYPE(Job)

inline const QString&
Job::GetID() const
{
	return m_id;
}

inline const QString&
Job::GetHost() const
{
	return m_host;
}

inline const QString&
Job::GetBucketName() const
{
	return m_bucketName;
}

inline uint64_t
Job::GetSize() const
{
	return m_size;
}

inline uint64_t
Job::GetBytesTransferred() const
{
	return m_bytesTransferred;
}

inline void
Job::SetID(const QString& id)
{
	m_id = id;
}

inline void
Job::SetHost(const QString& host)
{
	m_host = host;
}

inline void
Job::SetBucketName(const QString& bucketName)
{
	m_bucketName = bucketName;
}

inline void
Job::SetSize(uint64_t size)
{
	m_size = size;
}

inline void
Job::SetBytesTransferred(uint64_t bytesTransferred)
{
	m_bytesTransferred = bytesTransferred;
}

#endif
