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

#include "helpers/number_helper.h"

const uint64_t NumberHelper::B = 1;
const uint64_t NumberHelper::KB = NumberHelper::B * 1024;
const uint64_t NumberHelper::MB = NumberHelper::KB * 1024;
const uint64_t NumberHelper::GB = NumberHelper::MB * 1024;
const uint64_t NumberHelper::TB = NumberHelper::GB * 1024;

QString
NumberHelper::ToHumanSize(uint64_t bytes)
{
	double dbytes = static_cast<double>(bytes);
	double num;
	QString units;
	int precision = 1;

	if (bytes >= TB) {
		num = static_cast<double>(dbytes / TB);
		units = "TB";
	} else if (bytes >= GB) {
		num = static_cast<double>(dbytes / GB);
		units = "GB";
	} else if (bytes >= MB) {
		num = static_cast<double>(dbytes / MB);
		units = "MB";
	} else if (bytes >= KB) {
		num = static_cast<double>(dbytes / KB);
		units = "KB";
		precision = 0;
	} else {
		num = dbytes;
		units = "Bytes";
		precision = 0;
	}

	return (QString::number(num, 'f', precision) + " " + units);
}
