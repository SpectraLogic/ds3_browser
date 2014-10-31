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

#ifndef SESSION_H
#define SESSION_H

#include <string>

// Session, a model that represents the data necessary to setup a
// host<->DS3 session.
class Session
{
public:
	Session();
	Session(const std::string& host,
		const std::string& port,
		const std::string& accessId,
		const std::string& secretKey);

	std::string GetHost() const;
	void SetHost(const std::string& host);

	std::string GetPort() const;
	void SetPort(const std::string& port);

	std::string GetAccessId() const;
	void SetAccessId(const std::string& accessId);

	std::string GetSecretKey() const;
	void SetSecretKey(const std::string& secretKey);

private:
	std::string m_host;
	std::string m_port;
	std::string m_accessId;
	std::string m_secretKey;
};

inline std::string
Session::GetHost() const
{
	return m_host;
}

inline void
Session::SetHost(const std::string& host)
{
	m_host = host;
}

inline std::string
Session::GetPort() const
{
	return m_port;
}

inline void
Session::SetPort(const std::string& port)
{
	m_port = port;
}

inline std::string
Session::GetAccessId() const
{
	return m_accessId;
}

inline void
Session::SetAccessId(const std::string& accessId)
{
	m_accessId = accessId;
}

inline std::string
Session::GetSecretKey() const
{
	return m_secretKey;
}

inline void
Session::SetSecretKey(const std::string& secretKey)
{
	m_secretKey = secretKey;
}

#endif
