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
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QHash>

#include "lib/client.h"
#include "lib/logger.h"
#include "models/session.h"

// The S3 server imposes this limit
const uint64_t Client::MAX_NUM_BULK_PUT_OBJECTS = 500000;

static size_t read_from_qfile(void* buffer, size_t size, size_t count, void* user_data);

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

// TODO Paginate request if more than MAX_NUM_BULK_PUT_OBJECTS files
void
Client::BulkPut(const QString& bucketName,
		const QString& prefix,
		const QList<QUrl> urls)
{
	uint64_t numFiles = 0;
	QString truncMsg = "Truncating request to " + \
			   QString::number(MAX_NUM_BULK_PUT_OBJECTS) + \
			   " objects";
	QHash<QString, QString> objMap;
	QString normPrefix = prefix;
	if (!normPrefix.isEmpty()) {
		normPrefix.replace(QRegExp("/$"), "");
		normPrefix += "/";
	}
	int i;
	for (i = 0; i < urls.count(); i++) {
		if (numFiles >= MAX_NUM_BULK_PUT_OBJECTS) {
			LOG_WARNING(truncMsg);
			break;
		}
		numFiles++;
		QString filePath = urls[i].toLocalFile();
		// filePath could be either /foo or /foo/ if it's a directory.
		// Run it through QDir to normalize it to the former.
		filePath = QDir(filePath).path();
		QFileInfo fileInfo(filePath);
		QString fileName = fileInfo.fileName();
		QString objName = normPrefix + fileName;
		if (fileInfo.isDir()) {
			objName += "/";

			QDirIterator it(filePath,
					QDir::AllDirs | QDir::Files | QDir::Hidden | QDir::Readable | QDir::System | QDir::NoDotAndDotDot,
					QDirIterator::Subdirectories);
			while (it.hasNext()) {
				if (numFiles >= MAX_NUM_BULK_PUT_OBJECTS) {
					LOG_WARNING(truncMsg);
					break;
				}
				numFiles++;
				QString subFilePath = it.next();
				QFileInfo subFileInfo = it.fileInfo();
				QString subFileName = subFilePath;
				subFileName.replace(QRegExp("^" + filePath + "/"), "");
				QString subObjName = objName + subFileName;
				if (subFileInfo.isDir()) {
					subObjName += "/";
				}
				objMap.insert(subObjName, subFilePath);
			}
		}
		objMap.insert(objName, filePath);
	}

	ds3_bulk_object_list *bulkObjList = ds3_init_bulk_object_list(numFiles);

	QHash<QString, QString>::const_iterator hi;
	i = 0;
	for (hi = objMap.constBegin(); hi != objMap.constEnd(); hi++) {
		ds3_bulk_object* bulkObj = &bulkObjList->list[i];
		QString objName = hi.key();
		QString filePath = hi.value();
		QFileInfo fileInfo(filePath);
		uint64_t fileSize = 0;
		if (!fileInfo.isDir()) {
			fileSize = fileInfo.size();
		}
		bulkObj->name = ds3_str_init(objName.toUtf8().constData());
		bulkObj->length = fileSize;
		bulkObj->offset = 0;
		i++;
	}

	ds3_request* request = ds3_init_put_bulk(bucketName.toLocal8Bit().constData(), bulkObjList);
	ds3_bulk_response *response = NULL;
	ds3_error* error = ds3_bulk(m_client, request, &response);
	ds3_free_request(request);
	ds3_free_bulk_object_list(bulkObjList);

	if (error) {
		// TODO Handle the error
		ds3_free_error(error);
	}

	if (response == NULL) {
		// Bulk putting only empty folders will result in a 204
		// response (no content) indicating there's nothing else to do.
		return;
	}

	for (size_t j = 0; j < response->list_size; j++) {
		ds3_bulk_object_list* list = response->list[j];
		for (uint64_t k = 0; k < list->size; k++) {
			ds3_bulk_object* bulkObj = &list->list[k];
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
		QFile file(fileName);
		if (file.open(QIODevice::ReadOnly)) {
			error = ds3_put_object(m_client, request, &file, read_from_qfile);
			file.close();
		} else {
			LOG_ERROR("PUT object failed: unable to open file " + fileName);
		}
	}
	ds3_free_request(request);

	if (error) {
		// TODO Handle the error
		ds3_free_error(error);
	}
}

static size_t
read_from_qfile(void* buffer, size_t size, size_t count, void* user_data)
{
	QFile* qfile = (QFile*)user_data;
	return (qfile->read((char*)buffer, size * count));
}
