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

#include "models/session.h"

Session::Session()
{
}

Session::Session(const std::string& host,
		 const std::string& port,
		 const std::string& accessId,
		 const std::string& secretKey)
	: m_host(host),
	  m_port(port),
	  m_accessId(accessId),
	  m_secretKey(secretKey)
{
}
