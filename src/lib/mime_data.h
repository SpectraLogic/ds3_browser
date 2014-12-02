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

#ifndef MIME_DATA_H
#define MIME_DATA_H

#include <QList>
#include <QMimeData>
#include <QUrl>

// MimeData, a QMimeData class that recognizes the custom MIME type we use
// for DS3 bucket/object URLs.
class MimeData : public QMimeData
{
	Q_OBJECT

public:
	static const QString DS3_MIME_TYPE;

	bool HasDS3URLs() const;
	QList<QUrl> GetDS3URLs() const;
	void SetDS3URLs(const QList<QUrl>& urls);
};

inline bool
MimeData::HasDS3URLs() const
{
	return hasFormat(DS3_MIME_TYPE);
}

#endif
