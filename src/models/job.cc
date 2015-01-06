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

#include "models/job.h"

Job::Job()
	: m_state(INITIALIZING),
	  m_size(0),
	  m_bytesTransferred(0)
{
}

int
Job::GetProgress() const
{
	if (m_size == 0) {
		return 0;
	}

	int progress = (m_bytesTransferred * 1000) / m_size;
	if (progress < 0) {
		progress = 0;
	} else if (progress > 1000) {
		progress = 1000;
	}
	return progress;
}

