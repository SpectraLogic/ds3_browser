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

#include "models/ds3_url_test.h"
#include "models/ds3_url.h"

static DS3URLTest instance;

void
DS3URLTest::TestGetBucketName()
{
	QCOMPARE(DS3URL("http://host", "").GetBucketName(), QString(""));
	QCOMPARE(DS3URL("http://host", "/").GetBucketName(), QString(""));
	QCOMPARE(DS3URL("http://host", "a").GetBucketName(), QString("a"));
	QCOMPARE(DS3URL("http://host", "/a").GetBucketName(), QString("a"));
	QCOMPARE(DS3URL("http://host", "a/").GetBucketName(), QString("a"));
	QCOMPARE(DS3URL("http://host", "a/b").GetBucketName(), QString("a"));
	QCOMPARE(DS3URL("http://host", "a/b/").GetBucketName(), QString("a"));
}

void
DS3URLTest::TestGetObjectName()
{
	QCOMPARE(DS3URL("http://host", "").GetObjectName(), QString(""));
	QCOMPARE(DS3URL("http://host", "/").GetObjectName(), QString(""));
	QCOMPARE(DS3URL("http://host", "a").GetObjectName(), QString(""));
	QCOMPARE(DS3URL("http://host", "/a").GetObjectName(), QString(""));
	QCOMPARE(DS3URL("http://host", "a/").GetObjectName(), QString(""));
	QCOMPARE(DS3URL("http://host", "a/b").GetObjectName(), QString("b"));
	QCOMPARE(DS3URL("http://host", "a/b/").GetObjectName(), QString("b/"));
	QCOMPARE(DS3URL("http://host", "a/b/c").GetObjectName(), QString("b/c"));
	QCOMPARE(DS3URL("http://host", "a/b/c/").GetObjectName(), QString("b/c/"));

	QCOMPARE(DS3URL("http://host", "a/b?c=d&e=f").GetObjectName(), QString("b?c=d&e=f"));
	QCOMPARE(DS3URL("http://host", "a/b??c=d&e=f").GetObjectName(), QString("b??c=d&e=f"));
	QCOMPARE(DS3URL("http://host", "a/b%%20 c").GetObjectName(), QString("b%%20 c"));
}

void
DS3URLTest::TestGetLastPathPart()
{
	QCOMPARE(DS3URL("http://host", "").GetLastPathPart(), QString(""));
	QCOMPARE(DS3URL("http://host", "a").GetLastPathPart(), QString("a"));
	QCOMPARE(DS3URL("http://host", "a/").GetLastPathPart(), QString("a/"));
	QCOMPARE(DS3URL("http://host", "a/b").GetLastPathPart(), QString("b"));
	QCOMPARE(DS3URL("http://host", "a/b/").GetLastPathPart(), QString("b/"));
	QCOMPARE(DS3URL("http://host", "a/b/c").GetLastPathPart(), QString("c"));
	QCOMPARE(DS3URL("http://host", "a/b/c/").GetLastPathPart(), QString("c/"));

	QCOMPARE(DS3URL("http://host", "a/b/c?d=e&f=g").GetLastPathPart(), QString("c?d=e&f=g"));
	QCOMPARE(DS3URL("http://host", "a/b/c??d=e&f=g").GetLastPathPart(), QString("c??d=e&f=g"));
	QCOMPARE(DS3URL("http://host", "a/b/c%%20 d").GetLastPathPart(), QString("c%%20 d"));
}

void
DS3URLTest::TestIsBucket()
{
	QVERIFY(DS3URL("http://host", "a").IsBucket());
	QVERIFY(DS3URL("http://host", "a/").IsBucket());
	QVERIFY(!DS3URL("http://host", "a/b").IsBucket());
	QVERIFY(!DS3URL("http://host", "a/b/").IsBucket());
}

void
DS3URLTest::TestIsBucketOrFolder()
{
	QVERIFY(DS3URL("http://host", "a").IsBucketOrFolder());
	QVERIFY(DS3URL("http://host", "a/").IsBucketOrFolder());
	QVERIFY(!DS3URL("http://host", "a/b").IsBucketOrFolder());
	QVERIFY(DS3URL("http://host", "a/b/").IsBucketOrFolder());
}
