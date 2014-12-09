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

#ifndef DS3_URL_H
#define DS3_URL_H

#include <QString>
#include <QUrl>

// A QUrl convenience class that makes it easy to do things like pull
// the bucket name out of a DS3-based URL.
class DS3URL : public QUrl
{
public:
	static const QString PATH_REGEX;

	DS3URL();
	DS3URL(const QUrl& other);
	DS3URL(const QString& url, ParsingMode parsingMode = TolerantMode);

	QString GetBucketName() const;
	QString GetObjectName() const;
	QString GetLastPathPart() const;
	bool IsBucket() const;
	bool IsBucketOrFolder() const;
};

#endif
