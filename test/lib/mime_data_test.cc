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

#include <QList>
#include <QUrl>

#include "lib/mime_data_test.h"
#include "lib/mime_data.h"

static MimeDataTest instance;

void
MimeDataTest::TestURLRoundTrip()
{
	MimeData data;
	QVERIFY(!data.HasDS3URLs());

	QList<QUrl> urls;
	urls << QUrl("http://endpoint/bucket/object1");
	urls << QUrl("http://endpoint/bucket/object2");
	data.SetDS3URLs(urls);
	QVERIFY(data.HasDS3URLs());

	QList<QUrl> urls2 = data.GetDS3URLs();
	QCOMPARE(urls2.at(0), urls.at(0));
	QCOMPARE(urls2.at(1), urls.at(1));
}
