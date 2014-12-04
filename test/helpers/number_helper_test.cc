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

#include <QString>

#include "helpers/number_helper_test.h"
#include "helpers/number_helper.h"

static NumberHelperTest instance;

void
NumberHelperTest::TestToHumanSize()
{
	QCOMPARE(NumberHelper::ToHumanSize(0), QString("0 Bytes"));
	QCOMPARE(NumberHelper::ToHumanSize(10), QString("10 Bytes"));

	QCOMPARE(NumberHelper::ToHumanSize(1024), QString("1 KB"));
	QCOMPARE(NumberHelper::ToHumanSize(1536), QString("2 KB"));

	QCOMPARE(NumberHelper::ToHumanSize(1024 * 1024),
		 QString("1.0 MB"));
	QCOMPARE(NumberHelper::ToHumanSize(1024 * 1024 * 1.5),
		 QString("1.5 MB"));

	QCOMPARE(NumberHelper::ToHumanSize(1024 * 1024 * 1024),
		 QString("1.0 GB"));
	QCOMPARE(NumberHelper::ToHumanSize(1024 * 1024 * 1024 * 1.5),
		 QString("1.5 GB"));

	QCOMPARE(NumberHelper::ToHumanSize(1024 * 1024 * 1024 * 1024LLU),
		 QString("1.0 TB"));
	QCOMPARE(NumberHelper::ToHumanSize(1024 * 1024 * 1024 * 1024LLU * 1.5),
		 QString("1.5 TB"));
}
