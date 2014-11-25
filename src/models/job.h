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
#include <QUuid>

// Job, a container class that is a higher-level version of BulkWorkItem that's
// intended to be used by the Client to report job progress updates to the GUI
// components (e.g. DS3Browser).  This holds the minimum information necessary
// to report progress updates to the user.
class Job
{
public:
	Job();

	// FINISHED currently means the job has been successfully transferred
	// to the DS3 system's cache and not necessarily it's backing tape
	// store.
	enum State { INITIALIZING, QUEUED, PREPARING, INPROGRESS, FINISHED };

	const QUuid GetID() const;
	State GetState() const;
	const QString& GetHost() const;
	const QString& GetBucketName() const;
	uint64_t GetSize() const;
	uint64_t GetBytesTransferred() const;
	int GetProgress() const;
	bool IsFinished() const;

	void SetID(const QUuid& id);
	void SetState(State state);
	void SetHost(const QString& host);
	void SetBucketName(const QString& bucketName);
	void SetSize(uint64_t);
	void SetBytesTransferred(uint64_t);

private:
	QUuid m_id;
	State m_state;
	QString m_host;
	QString m_bucketName;
	uint64_t m_size;
	uint64_t m_bytesTransferred;
};

// Job is used as an argument in a signal/slot connection
Q_DECLARE_METATYPE(Job)

inline const QUuid
Job::GetID() const
{
	return m_id;
}

inline Job::State
Job::GetState() const
{
	return m_state;
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

inline bool Job::IsFinished() const
{
	return m_state == FINISHED;
}

inline void
Job::SetID(const QUuid& id)
{
	m_id = id;
}

inline void
Job::SetState(State state)
{
	m_state = state;
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
