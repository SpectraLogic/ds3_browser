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

#include <QRegExp>

#include "models/ds3_url.h"

const QString DS3URL::PATH_REGEX = "^/?([^/]+)/?(?:/(.*))?$";

DS3URL::DS3URL()
	: QUrl()
{
}

DS3URL::DS3URL(const QUrl& other)
	: QUrl(other)
{
}

DS3URL::DS3URL(const QString& url, ParsingMode parsingMode)
	: QUrl(url, parsingMode)
{
}

QString
DS3URL::GetBucketName() const
{
	QRegExp regex(PATH_REGEX);
	QString bucketName;
	int pos = regex.indexIn(path());
	if (pos > -1) {
		bucketName = regex.cap(1);
	}
	return bucketName;
}

QString
DS3URL::GetObjectName() const
{
	QRegExp regex(PATH_REGEX);
	QString objName;
	int pos = regex.indexIn(path());
	if (pos > -1) {
		objName = regex.cap(2);
	}
	return objName;
}

QString
DS3URL::GetLastPathPart() const
{
	QRegExp regex("([^/]+/?)$");
	QString part;
	int pos = regex.indexIn(path());
	if (pos > -1) {
		QString match = regex.cap(1);
		part = match;
	}
	return part;
}

bool
DS3URL::IsBucket() const
{
	QString objName = GetObjectName();
	return (objName.isEmpty() || objName == "/");
}

bool
DS3URL::IsBucketOrFolder() const
{
	QString objName = GetObjectName();
	return (objName.isEmpty() || path().endsWith("/"));
}
