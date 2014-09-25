#ifndef SESSION_H
#define SESSION_H

#include <string>

class Session
{
public:
	Session();
	Session(std::string, int, std::string, std::string);

	std::string GetHost();
	void SetHost(const std::string);

	int GetPort();
	void SetPort(const int);

	std::string GetAccessId();
	void SetAccessId(const std::string);

	std::string GetSecretKey();
	void SetSecretKey(const std::string);

private:

	std::string m_host;

	int m_port;

	std::string m_accessId;

	std::string m_secretKey;
};

inline std::string
Session::GetHost()
{
	return m_host;
}

inline void
Session::SetHost(const std::string host)
{
	m_host = host;
}

inline int
Session::GetPort()
{
	return m_port;
}

inline void
Session::SetPort(const int port)
{
	m_port = port;
}

inline std::string
Session::GetAccessId()
{
	return m_accessId;
}

inline void
Session::SetAccessId(const std::string accessId)
{
	m_accessId = accessId;
}

inline std::string
Session::GetSecretKey()
{
	return m_secretKey;
}

inline void
Session::SetSecretKey(const std::string secretKey)
{
	m_secretKey = secretKey;
}

#endif
