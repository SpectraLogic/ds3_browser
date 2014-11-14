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

#include <QList>
#include <QString>
#include <QUrl>

#include <ds3.h>

class Session;

class Client
{
public:
	static const uint64_t MAX_NUM_BULK_PUT_OBJECTS;

	Client(const Session* session);
	~Client();

	ds3_get_service_response* GetService();
	ds3_get_bucket_response* GetBucket(const QString& bucketName,
					   const QString& prefix,
					   const QString& delimiter,
					   const QString& marker,
					   uint32_t maxKeys = 0);

	void CreateBucket(const QString& name);

	void BulkPut(const QString& bucketName,
		     const QString& prefix,
		     const QList<QUrl> urls);

	void PutObject(const QString& bucket,
		       const QString& object,
		       const QString& fileName);

private:
	QString m_endpoint;
	ds3_creds* m_creds;
	ds3_client* m_client;
};

#endif
