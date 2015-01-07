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

#include <QStringList>

#include "lib/errors/ds3_error.h"

DS3Error::DS3Error(ds3_error* error)
	: QException(),
	  m_code(error->code),
	  m_statusCode(0)
{
	if (error->message != NULL) {
		m_message = QString::fromUtf8(error->message->value);
	}

	ds3_error_response* errorRes = error->error;
	if (errorRes != NULL) {
		m_statusCode = errorRes->status_code;
		ds3_str* ds3Str = errorRes->status_message;
		if (ds3Str != NULL) {
			m_statusMessage = QString::fromUtf8(ds3Str->value);
		}
		ds3Str = errorRes->error_body;
		if (ds3Str != NULL) {
			m_errorBody = QString::fromUtf8(ds3Str->value);
		}
	}
}

QString
DS3Error::ToString() const
{
	QStringList str;

	if (m_code != DS3_ERROR_BAD_STATUS_CODE) {
		return m_message;
	}

	QString body = GetErrorBody();
	QString msg;
	switch (m_statusCode) {
	case 403:
		if (body.contains("clock is not synchronized")) {
			msg = "Client clock is not synchronized " \
			      "with server clock";
		} else {
			msg = "Invalid S3 Access ID or S3 Secret Key";
		}
		return msg;
	}

	if (m_statusCode != 0) {
		str << "Status Code: " + QString::number(m_statusCode);
	}
	if (!m_statusMessage.isEmpty()) {
		str << "Error Message: " + m_statusMessage;
	}
	return str.join(", ");
}
