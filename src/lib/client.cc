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
#include <QtConcurrent>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QHash>

#include "lib/work_items/bulk_put_work_item.h"
#include "lib/client.h"
#include "lib/logger.h"
#include "models/session.h"

using QtConcurrent::run;

const QString Client::DELIMITER = "/";

// The S3 server imposes this limit although we might want to lower it
const uint64_t Client::BULK_PUT_PAGE_LIMIT = 500000;

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

QFuture<ds3_get_service_response*>
Client::GetService()
{
	QFuture<ds3_get_service_response*> future = run(this, &Client::DoGetService);
	return future;
}

QFuture<ds3_get_bucket_response*>
Client::GetBucket(const QString& bucketName, const QString& prefix,
		  const QString& marker, uint32_t maxKeys)
{
	QFuture<ds3_get_bucket_response*> future = run(this,
						       &Client::DoGetBucket,
						       bucketName,
						       prefix,
						       marker,
						       maxKeys);
	return future;
}

void
Client::CreateBucket(const QString& name)
{
	ds3_request* request = ds3_init_put_bucket(name.toUtf8().constData());
	LOG_INFO("Create Bucket " + name + " (PUT " + m_endpoint + "/" + \
		 name + ")");
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
	BulkPutWorkItem* workItem = new BulkPutWorkItem(bucketName, prefix, urls);
	run(this, &Client::PrepareBulkPuts, workItem);
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

ds3_get_service_response*
Client::DoGetService()
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
Client::DoGetBucket(const QString& bucketName, const QString& prefix,
		    const QString& marker, uint32_t maxKeys)
{
	ds3_get_bucket_response *response;
	ds3_request* request = ds3_init_get_bucket(bucketName.toUtf8().constData());
	QString logMsg = "List Objects (GET " + m_endpoint + "/";
	logMsg += bucketName;
	QStringList logQueryParams;
	if (!prefix.isEmpty()) {
		ds3_request_set_prefix(request, prefix.toUtf8().constData());
		logQueryParams << "prefix=" + prefix;
	}
	ds3_request_set_delimiter(request, DELIMITER.toUtf8().constData());
	logQueryParams << "delimiter=" + DELIMITER;
	if (!marker.isEmpty()) {
		ds3_request_set_marker(request, marker.toUtf8().constData());
		logQueryParams << "marker=" + marker;
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

// TODO Refactor to remove the duplicate directory iteration code
void
Client::PrepareBulkPuts(BulkPutWorkItem* workItem)
{
	LOG_DEBUG("PrepareBulkPuts");

	uint64_t numFiles = 0;
	workItem->ClearObjMap();
	QString normPrefix = workItem->GetPrefix();
	if (!normPrefix.isEmpty()) {
		normPrefix.replace(QRegExp("/$"), "");
		normPrefix += "/";
	}


	QDirIterator* di = workItem->GetDirIterator();
	if (di != NULL) {
		// Previous page left off while in the middle of iterating
		// through a URL's directory tree.  Finish it up.

		QList<QUrl>::const_iterator& ui(workItem->GetUrlsIterator());
		QString filePath = (*ui).toLocalFile();
		// filePath could be either /foo or /foo/ if it's a directory.
		// Run it through QDir to normalize it to the former.
		filePath = QDir(filePath).path();
		QFileInfo fileInfo(filePath);
		QString fileName = fileInfo.fileName();
		// The URL the iterator is pointing to must be a directory
		// of the directory iterator wasn't null.
		QString objName = normPrefix + fileName + "/";
		while (di->hasNext()) {
			if (numFiles >= BULK_PUT_PAGE_LIMIT) {
				run(this, &Client::DoBulkPut, workItem);
				return;
			}
			numFiles++;
			QString subFilePath = di->next();
			QFileInfo subFileInfo = di->fileInfo();
			QString subFileName = subFilePath;
			subFileName.replace(QRegExp("^" + filePath + "/"), "");
			QString subObjName = objName + subFileName;
			if (subFileInfo.isDir()) {
				subObjName += "/";
			}
			LOG_DEBUG("Inserting " + subObjName + ", " + subFilePath);
			workItem->InsertObjMap(subObjName, subFilePath);
		}
		workItem->DeleteDirIterator();
		ui++;
	}


	for (QList<QUrl>::const_iterator& ui(workItem->GetUrlsIterator());
	     ui != workItem->GetUrlsConstEnd();
	     ui++) {
		if (numFiles >= BULK_PUT_PAGE_LIMIT) {
			run(this, &Client::DoBulkPut, workItem);
			return;
		}
		numFiles++;
		QString filePath = (*ui).toLocalFile();
		// filePath could be either /foo or /foo/ if it's a directory.
		// Run it through QDir to normalize it to the former.
		filePath = QDir(filePath).path();
		QFileInfo fileInfo(filePath);
		QString fileName = fileInfo.fileName();
		QString objName = normPrefix + fileName;
		bool fileIsDir = fileInfo.isDir();
		if (fileIsDir) {
			objName += "/";
		}
		LOG_DEBUG("Inserting " + objName + ", " + filePath);
		workItem->InsertObjMap(objName, filePath);
		if (fileIsDir) {
			QDirIterator* di = workItem->GetDirIterator(filePath);
			while (di->hasNext()) {
				if (numFiles >= BULK_PUT_PAGE_LIMIT) {
					run(this, &Client::DoBulkPut, workItem);
					return;
				}
				numFiles++;
				QString subFilePath = di->next();
				QFileInfo subFileInfo = di->fileInfo();
				QString subFileName = subFilePath;
				subFileName.replace(QRegExp("^" + filePath + "/"), "");
				QString subObjName = objName + subFileName;
				if (subFileInfo.isDir()) {
					subObjName += "/";
				}
				LOG_DEBUG("Inserting " + subObjName + ", " + subFilePath);
				workItem->InsertObjMap(subObjName, subFilePath);
			}
			workItem->DeleteDirIterator();
		}
	}

	if (numFiles > 0) {
		run(this, &Client::DoBulkPut, workItem);
	}
}

void
Client::DoBulkPut(BulkPutWorkItem* workItem)
{
	LOG_DEBUG("DoBulkPuts");

	uint64_t numFiles = workItem->GetObjMapSize();
	ds3_bulk_object_list *bulkObjList = ds3_init_bulk_object_list(numFiles);

	QHash<QString, QString>::const_iterator hi;
	int i = 0;
	for (hi = workItem->GetObjMapConstBegin(); hi != workItem->GetObjMapConstEnd(); hi++) {
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

	const QString& bucketName = workItem->GetBucketName();
	ds3_request* request = ds3_init_put_bulk(bucketName.toUtf8().constData(), bulkObjList);
	ds3_bulk_response *response = NULL;
	ds3_error* error = ds3_bulk(m_client, request, &response);
	ds3_free_request(request);
	ds3_free_bulk_object_list(bulkObjList);
	workItem->SetResponse(response);

	if (error) {
		// TODO Handle the error
		LOG_ERROR("BulkPut Error");
		ds3_free_error(error);
	}

	if (response == NULL) {
		// Bulk putting only empty folders will result in a 204
		// response (no content) indicating there's nothing else to do.
		LOG_DEBUG("No objects to put.  Deleting work item");
		delete workItem;
		return;
	}

	for (size_t j = 0; j < response->list_size; j++) {
		LOG_DEBUG("Starting PutBulkOjbectList thread");
		ds3_bulk_object_list* list = response->list[j];
		workItem->IncWorkingObjListCount();
		run(this, &Client::PutBulkObjectList, workItem, list);
	}
}

void
Client::PutBulkObjectList(BulkPutWorkItem* workItem,
			  const ds3_bulk_object_list* list)
{
	QString bucketName = workItem->GetBucketName();
	for (uint64_t k = 0; k < list->size; k++) {
		ds3_bulk_object* bulkObj = &list->list[k];
		QString objName = QString(bulkObj->name->value);
		QString filePath = workItem->GetObjMapValue(objName);
		PutObject(bucketName, objName, filePath);
	}
	workItem->DecWorkingObjListCount();

	if (workItem->IsPageFinished()) {
		if (workItem->IsFinished()) {
			LOG_DEBUG("Finished with bulk put work item.  Deleting it.");
			delete workItem;
		} else {
			LOG_DEBUG("More bulk put pages to go.  Starting PrepareBulkPuts again.");
			run(this, &Client::PrepareBulkPuts, workItem);
		}
	} else {
		LOG_DEBUG("PutBulkObjlistList done but more still running");
	}
}

static size_t
read_from_qfile(void* buffer, size_t size, size_t count, void* user_data)
{
	QFile* qfile = (QFile*)user_data;
	return (qfile->read((char*)buffer, size * count));
}
