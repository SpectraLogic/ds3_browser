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

#ifndef WORK_ITEM_H
#define WORK_ITEM_H

#include <QDateTime>
#include <QUuid>

// WorkItem, a container class that holds data necessary for Client to perform
// a DS3 request and handle its response.  These are only intended to be used
// by the Client and not necessarily the GUI.
class WorkItem
{
public:
	WorkItem();
	virtual ~WorkItem();

	const QUuid GetID() const;
	const QDateTime& GetStart() const;
	const QDateTime& GetTransferStart() const;

	void SetTransferStartIfNull();

private:
	QUuid m_id;
	// Time the WorkItem was created
	QDateTime m_start;
	// Time the actual transfer started, which could be different than
	// m_start if a lot of prep time is required.
	QDateTime m_transferStart;
};

inline const QUuid
WorkItem::GetID() const
{
	return m_id;
}

inline const QDateTime&
WorkItem::GetStart() const
{
	return m_start;
}

inline const QDateTime&
WorkItem::GetTransferStart() const
{
	return m_transferStart;
}

inline void
WorkItem::SetTransferStartIfNull()
{
	if (m_transferStart.isNull()) {
		m_transferStart = QDateTime::currentDateTime();
	}
}

#endif
