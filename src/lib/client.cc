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

ds3_get_bucket_response*
Client::GetBucket(std::string bucketName,
		  std::string prefix,
		  std::string delimiter,
		  std::string nextMarker)
{
	ds3_get_bucket_response *response;
	ds3_request* request = ds3_init_get_bucket(bucketName.c_str());
	if (!prefix.empty()) {
		ds3_request_set_prefix(request, prefix.c_str());
	}
	if (!delimiter.empty()) {
		ds3_request_set_delimiter(request, delimiter.c_str());
	}
	if (!nextMarker.empty()) {
		ds3_request_set_next_marker(request, nextMarker.c_str());
	}
	ds3_error* error = ds3_get_bucket(m_client,
					  request,
					  &response);
	ds3_free_request(request);

	// TODO check the error
	ds3_free_error(error);

	return response;
}

void
Client::CreateBucket(std::string name)
{
	ds3_request* request = ds3_init_put_bucket(name.c_str());
	ds3_error* error = ds3_put_bucket(m_client, request);
	ds3_free_request(request);

	// TODO check the error
	ds3_free_error(error);

	return;
}
