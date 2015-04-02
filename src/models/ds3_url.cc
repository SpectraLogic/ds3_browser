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

#include <QRegularExpression>

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

DS3URL::DS3URL(const QString& endpoint, const QString& decodedPath)
	: QUrl(endpoint)
{
	setPath(decodedPath);
}

QString
DS3URL::GetBucketName() const
{
	QRegularExpression regex(PATH_REGEX);
	QString bucketName;
	QRegularExpressionMatch match = regex.match(path());
	if (match.hasMatch()) {
		bucketName = match.captured(1);
	}
	return bucketName;
}

QString
DS3URL::GetObjectName() const
{
	QRegularExpression regex(PATH_REGEX);
	QString objName;
	QRegularExpressionMatch match = regex.match(path());
	if (match.hasMatch()) {
		objName = match.captured(2);
	}
	return objName;
}

QString
DS3URL::GetLastPathPart() const
{
	QRegularExpression regex("([^/]+/?)$");
	QString part;
	QRegularExpressionMatch match = regex.match(path());
	if (match.hasMatch()) {
		part = match.captured(1);
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
