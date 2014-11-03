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

#include <QString>

// Session, a model that represents the data necessary to setup a
// host<->DS3 session.
class Session
{
public:
	enum Protocol { HTTP, HTTPS };
	static const QString PROTOCOL_NAMES[];

	Session();

	QString GetHost() const;
	void SetHost(const QString& host);

	Protocol GetProtocol() const;
	QString GetProtocolName() const;
	void SetProtocol(Protocol protocol);
	void SetProtocol(int protocol);

	QString GetPort() const;
	void SetPort(const QString& port);

	bool GetWithCertificateVerification() const;
	void SetWithCertificateVerification(bool verify);

	QString GetAccessId() const;
	void SetAccessId(const QString& accessId);

	QString GetSecretKey() const;
	void SetSecretKey(const QString& secretKey);

private:
	QString m_host;
	Protocol m_protocol;
	QString m_port;
	// Whether or not SSL certificates should be verified.  This is only
	// applicable when using HTTPS.  If this is set to true, the user
	// would have to configure their computer to trust their system's
	// SSL certificate.
	bool m_withCertificateVerification;
	QString m_accessId;
	QString m_secretKey;
};

inline QString
Session::GetHost() const
{
	return m_host;
}

inline void
Session::SetHost(const QString& host)
{
	m_host = host;
}

inline Session::Protocol
Session::GetProtocol() const
{
	return m_protocol;
}

inline QString
Session::GetProtocolName() const
{
	return PROTOCOL_NAMES[m_protocol];
}

inline void
Session::SetProtocol(Session::Protocol protocol)
{
	m_protocol = protocol;
}

inline void
Session::SetProtocol(int protocol)
{
	m_protocol = static_cast<Protocol>(protocol);
}

inline QString
Session::GetPort() const
{
	return m_port;
}

inline void
Session::SetPort(const QString& port)
{
	m_port = port;
}

inline bool
Session::GetWithCertificateVerification() const
{
	return m_withCertificateVerification;
}

inline void
Session::SetWithCertificateVerification(bool verify)
{
	m_withCertificateVerification = verify;
}

inline QString
Session::GetAccessId() const
{
	return m_accessId;
}

inline void
Session::SetAccessId(const QString& accessId)
{
	m_accessId = accessId;
}

inline QString
Session::GetSecretKey() const
{
	return m_secretKey;
}

inline void
Session::SetSecretKey(const QString& secretKey)
{
	m_secretKey = secretKey;
}

#endif
