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

#include "lib/mime_data.h"

const QString MimeData::DS3_MIME_TYPE = "text/spectra-ds3-uri-list";


QList<QUrl>
MimeData::GetDS3URLs() const
{
	QList<QUrl> urls;
	QByteArray encodedUrls = data(DS3_MIME_TYPE);
	QDataStream stream(&encodedUrls, QIODevice::ReadOnly);
	while (!stream.atEnd()) {
		QUrl url;
		stream >> url;
		urls << url;
	}
	return urls;
}

void
MimeData::SetDS3URLs(const QList<QUrl>& urls)
{
	QByteArray encodedUrls;
	QDataStream stream(&encodedUrls, QIODevice::WriteOnly);
	for (int i = 0; i < urls.size(); i++) {
		stream << urls.at(i);
	}
	setData(DS3_MIME_TYPE, encodedUrls);
}
