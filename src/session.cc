#include "session.h"

Session::Session()
{
}

Session::Session(std::string host,
		 int port,
		 std::string accessId,
		 std::string secretKey)
	: m_host(host),
	  m_port(port),
	  m_accessId(accessId),
	  m_secretKey(secretKey)
{
}
