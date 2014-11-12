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

#include <stdlib.h>
#include <string.h>
#include <QFileInfo>
#include <QHash>

#include "lib/client.h"
#include "lib/logger.h"
#include "models/session.h"

Client::Client(const Session* session)
{
	m_creds = ds3_create_creds(session->GetAccessId().toUtf8().constData(),
				   session->GetSecretKey().toUtf8().constData());

	QString protocol = session->GetProtocolName();
	m_endpoint = protocol + "://" + session->GetHost();
	QString port = session->GetPort();
	if (!port.isEmpty() && port != "80" && port != "443") {
		m_endpoint += ":" + port;
	}

	m_client = ds3_create_client(m_endpoint.toUtf8().constData(), m_creds);
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
	LOG_INFO("Get Buckets (GET " + m_endpoint + ")");
	ds3_error* error = ds3_get_service(m_client,
					   request,
					   &response);
	ds3_free_request(request);

	if (error) {
		// TODO Handle the error
		ds3_free_error(error);
	}

	return response;
}

ds3_get_bucket_response*
Client::GetBucket(const std::string& bucketName,
		  const std::string& prefix,
		  const std::string& delimiter,
		  const std::string& marker,
		  uint32_t maxKeys)
{
	ds3_get_bucket_response *response;
	ds3_request* request = ds3_init_get_bucket(bucketName.c_str());
	QString logMsg = "List Objects (GET " + m_endpoint + "/";
	logMsg += QString::fromStdString(bucketName);
	QStringList logQueryParams;
	if (!prefix.empty()) {
		ds3_request_set_prefix(request, prefix.c_str());
		logQueryParams << "prefix=" + QString::fromStdString(prefix);
	}
	if (!delimiter.empty()) {
		ds3_request_set_delimiter(request, delimiter.c_str());
		logQueryParams << "delimiter=" + QString::fromStdString(delimiter);
	}
	if (!marker.empty()) {
		ds3_request_set_marker(request, marker.c_str());
		logQueryParams << "marker=" + QString::fromStdString(marker);
	}
	if (maxKeys > 0) {
		ds3_request_set_max_keys(request, maxKeys);
		logQueryParams << "max-keys=" + QString::number(maxKeys);
	}
	if (!logQueryParams.isEmpty()) {
		logMsg += "&" + logQueryParams.join("&");
	}
	logMsg += ")";
	LOG_INFO(logMsg);
	ds3_error* error = ds3_get_bucket(m_client,
					  request,
					  &response);
	ds3_free_request(request);

	if (error) {
		// TODO Handle the error
		ds3_free_error(error);
	}

	return response;
}

void
Client::CreateBucket(const std::string& name)
{
	ds3_request* request = ds3_init_put_bucket(name.c_str());
	QString qname = QString::fromStdString(name);
	LOG_INFO("Create Bucket " + qname + " (PUT " + m_endpoint + "/" + \
		 qname + ")");
	ds3_error* error = ds3_put_bucket(m_client, request);
	ds3_free_request(request);

	if (error) {
		// TODO Handle the error
		ds3_free_error(error);
	}
}


void
Client::BulkPut(const QString& bucketName,
		const QString& prefix,
		const QList<QUrl> urls)
{
	uint64_t numFiles = urls.count();
	ds3_bulk_object_list *bulkObjList = (ds3_bulk_object_list*)calloc(1, sizeof(ds3_bulk_object_list));
	bulkObjList->list = (ds3_bulk_object*)malloc(numFiles * sizeof(ds3_bulk_object));
	bulkObjList->size = numFiles;
	QHash<QString, QString> objMap;
	QString normPrefix = prefix;
	if (!normPrefix.isEmpty()) {
		normPrefix.replace(QRegExp("/$"), "");
		normPrefix += "/";
	}
	for (uint64_t i = 0; i < numFiles; i++) {
		QString filePath = urls[i].path();
		QFileInfo fileInfo(filePath);
		QString fileName = fileInfo.fileName();
		QString objName = normPrefix + fileName;
		uint64_t fileSize = 0;
		if (fileInfo.isDir()) {
			objName.replace(QRegExp("/$"), "");
			objName += "/";
			// TODO Recursively get all files in dir
		} else {
			fileSize = fileInfo.size();
		}
		objMap.insert(objName, filePath);
		ds3_bulk_object* bulkObj = &bulkObjList->list[i];
		bulkObj->name = ds3_str_init(objName.toUtf8().constData());
		bulkObj->length = fileSize;
		bulkObj->offset = 0;
	}

	ds3_request* request = ds3_init_put_bulk(bucketName.toLocal8Bit().constData(), bulkObjList);
	ds3_bulk_response *response;
	ds3_error* error = ds3_bulk(m_client, request, &response);
	ds3_free_request(request);
	ds3_free_bulk_object_list(bulkObjList);

	if (error) {
		// TODO Handle the error
		ds3_free_error(error);
	}

	for (size_t i = 0; i < response->list_size; i++) {
		ds3_bulk_object_list* list = response->list[i];
		for (uint64_t j = 0; j < list->size; j++) {
			ds3_bulk_object* bulkObj = &list->list[j];
			QString objName = QString(bulkObj->name->value);
			// TODO objMap only holds objects for URLs that were
			//      passed in and not files that were discovered
			//      underneath directories.  Account for that.
			QString filePath = objMap[objName];
			PutObject(bucketName, objName, filePath);
		}
	}

	ds3_free_bulk_response(response);
}

void
Client::PutObject(const QString& bucket,
		  const QString& object,
		  const QString& fileName)
{
	LOG_DEBUG("PUT OBJECT: " + object + ", FILE: " + fileName);

	QFileInfo fileInfo(fileName);
	ds3_request* request = ds3_init_put_object(bucket.toUtf8().constData(),
						   object.toUtf8().constData(),
						   fileInfo.size());
	ds3_error* error = NULL;
	if (fileInfo.isDir()) {
		// "folder" objects don't have a size nor do they have any
		// data associated with them
		error = ds3_put_object(m_client, request, NULL, NULL);
	} else {
		FILE* file = fopen(fileName.toUtf8().constData(), "r");
		if (file == NULL) {
			LOG_ERROR("PUT object failed: unable to open file " + fileName);
		} else {
			error = ds3_put_object(m_client, request,
					       file, ds3_read_from_file);
			fclose(file);
		}
	}
	ds3_free_request(request);

	if (error) {
		// TODO Handle the error
		ds3_free_error(error);
	}
}
