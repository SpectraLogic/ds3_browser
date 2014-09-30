#include "models/session.h"

Session::Session()
{
}

Session::Session(std::string host,
		 std::string port,
		 std::string accessId,
		 std::string secretKey)
	: m_host(host),
	  m_port(port),
	  m_accessId(accessId),
	  m_secretKey(secretKey)
{
}
