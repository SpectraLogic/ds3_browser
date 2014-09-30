#include "client.h"

Client::Client(std::string host,
	       std::string port,
	       std::string accessId,
	       std::string secretKey)
{
	m_creds = ds3_create_creds(accessId.c_str(), secretKey.c_str());

	std::string endpoint = "http://" + host + ":" + port;
	m_client = ds3_create_client(endpoint.c_str(), m_creds);
}

Client::~Client()
{
	ds3_free_creds(m_creds);
	ds3_free_client(m_client);
	ds3_cleanup();
}

ds3_get_service_response*
Client::GetService()
{
	ds3_get_service_response *response;
	ds3_request* request = ds3_init_get_service();
	ds3_error* error = ds3_get_service(m_client,
					   request,
					   &response);
	ds3_free_request(request);

	// TODO check the error
	ds3_free_error(error);

	return response;
}
