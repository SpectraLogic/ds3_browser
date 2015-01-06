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

#ifndef DS3_ERROR_H
#define DS3_ERROR_H

#include <QException>
#include <QString>

#include <ds3.h>

// DS3Error, an error class used for any C SDK request that results in an
// error.  It's a QException class so that it can be transferred across
// threads (e.g. thrown from a "QtConcurrent::run" thread and caught from
// the thread making a call to QFutureWatcher::result()).
class DS3Error : public QException
{
public:
	DS3Error(ds3_error* error);
	virtual ~DS3Error() throw() {};

	uint64_t GetStatusCode() const;
	QString GetErrorBody() const;

	QString ToString() const;

	// QException required overrides
	void raise() const { throw *this; }
	DS3Error* clone() const { return (new DS3Error(*this)); }

private:
	ds3_error_code m_code;
	QString m_message;

	uint64_t m_statusCode;
	QString m_statusMessage;
	QString m_errorBody;
};

inline uint64_t
DS3Error::GetStatusCode() const
{
	return m_statusCode;
}

inline QString
DS3Error::GetErrorBody() const
{
	return m_errorBody;
}

#endif
