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

#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#include <ds3.h>

class Client
{
public:
	Client(const std::string& host,
	       const std::string& port,
	       const std::string& accessId,
	       const std::string& secretKey);
	~Client();

	ds3_get_service_response* GetService();
	ds3_get_bucket_response* GetBucket(const std::string& bucketName,
					   const std::string& prefix = std::string(),
					   const std::string& delimiter = std::string(),
					   const std::string& marker = std::string(),
					   uint32_t maxKeys = 0);

	void CreateBucket(const std::string& name);

private:
	ds3_creds* m_creds;
	ds3_client* m_client;
};

#endif
