#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#include <ds3.h>

class Client
{
public:
	Client(std::string, std::string, std::string, std::string);
	~Client();

	ds3_get_service_response* GetService();

private:
	ds3_creds* m_creds;
	ds3_client* m_client;
};

#endif
