/*
 * *****************************************************************************
 *   Copyright 2014-2015 Spectra Logic Corporation. All Rights Reserved.
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
#include <QRegularExpression>

#include "lib/work_items/bulk_get_work_item.h"
#include "lib/work_items/bulk_put_work_item.h"
#include "lib/work_items/object_work_item.h"
#include "lib/client.h"
#include "lib/logger.h"
#include "models/ds3_url.h"
#include "models/session.h"

using QtConcurrent::run;

const QString Client::DELIMITER = "/";

// The S3 server imposes a max limit of 500,000 which could be lower depending
// on the amount of RAM in the server.
const uint64_t Client::BULK_PAGE_LIMIT = 100000;

// 0 = don't specify it in requests and let the S3 server determine the max
const uint32_t Client::MAX_KEYS = 0;

static size_t read_from_file(void* buffer, size_t size, size_t count, void* user_data);
static size_t write_to_file(void* buffer, size_t size, size_t count, void* user_data);

// Simple struct to wrap a Client and an ObjectWorkItem so the C SDK can
// send both to the file read/write callback functions.
struct ClientAndObjectWorkItem
{
	Client* client;
	ObjectWorkItem* objectWorkItem;
};

Client::Client(const Session* session)
{
	m_creds = ds3_create_creds(session->GetAccessId().toUtf8().constData(),
				   session->GetSecretKey().toUtf8().constData());

	QString protocol = session->GetProtocolName();
	m_host = session->GetHost();
	m_endpoint = protocol + "://" + m_host;
	QString port = session->GetPort();
	if (!port.isEmpty() && port != "80" && port != "443") {
		m_endpoint += ":" + port;
	}

	m_client = ds3_create_client(m_endpoint.toUtf8().constData(), m_creds);
	QString proxy = session->GetProxy();
	if (!proxy.isEmpty()) {
		ds3_client_proxy(m_client, proxy.toUtf8().constData());
	}
}

Client::~Client()
{
	ds3_free_creds(m_creds);
	ds3_free_client(m_client);
}

int
Client::GetNumActiveJobs() const
{
	m_bulkWorkItemsLock.lock();
	int size = m_bulkWorkItems.size();
	m_bulkWorkItemsLock.unlock();
	return size;
}

void
Client::CancelActiveJobs()
{
	m_bulkWorkItemsLock.lock();
	QHashIterator<QUuid, BulkWorkItem*> i(m_bulkWorkItems);
	while (i.hasNext()) {
		i.next();
		BulkWorkItem* workItem = i.value();
		Job::State state = workItem->GetState();
		if (state != Job::CANCELING && state != Job::CANCELED &&
		    state != Job::FINISHED) {
			workItem->SetState(Job::CANCELING);
		}
	}
	m_bulkWorkItemsLock.unlock();
}

QFuture<ds3_get_service_response*>
Client::GetService()
{
	QFuture<ds3_get_service_response*> future = run(this, &Client::DoGetService);
	return future;
}

QFuture<ds3_get_bucket_response*>
Client::GetBucket(const QString& bucketName, const QString& prefix,
		  const QString& marker, bool silent, const QString& delimiter)
{
	QFuture<ds3_get_bucket_response*> future = run(this,
						       &Client::DoGetBucket,
						       bucketName,
						       prefix,
						       delimiter,
						       marker,
						       silent);
	return future;
}

void
Client::CreateBucket(const QString& name)
{
	ds3_request* request = ds3_init_put_bucket(name.toUtf8().constData());
	LOG_INFO("PUT          BUCKET    "+m_endpoint+"/"+name);
	ds3_error* ds3Error = ds3_put_bucket(m_client, request);
	ds3_free_request(request);

	if (ds3Error != NULL) {
		DS3Error error(ds3Error);
		ds3_free_error(ds3Error);
		throw (error);
	}
}

void
Client::DeleteBucket(const QString& name)
{
	ds3_request* request = ds3_init_delete_bucket(name.toUtf8().constData());
	LOG_INFO("DELETE       BUCKET    "+m_endpoint+"/"+name);
	ds3_error* ds3Error = ds3_delete_bucket(m_client, request);
	ds3_free_request(request);

	if (ds3Error != NULL) {
		DS3Error error(ds3Error);
		ds3_free_error(ds3Error);
		throw (error);
	}
}

void
Client::DeleteObjects(const QString& bucketName, const QStringList& objectNames)
{
	ds3_request* request = ds3_init_delete_objects(bucketName.toUtf8().constData());
	LOG_INFO("DELETE       OBJECTS    "+bucketName+"/{"+objectNames.join(",")+"}");

	ds3_bulk_object_list *bulkObjList = ds3_init_bulk_object_list(objectNames.size());

	for (int i=0; i<objectNames.size(); i++) {
		ds3_bulk_object* bulkObj = &bulkObjList->list[i];
		QString objName = objectNames[i];
		bulkObj->name = ds3_str_init(objName.toUtf8().constData());
	}

	ds3_error* ds3Error = ds3_delete_objects(m_client, request, bulkObjList);

	ds3_free_request(request);

	if (ds3Error != NULL) {
		DS3Error error(ds3Error);
		ds3_free_error(ds3Error);
		throw (error);
	}
}

void
Client::DeleteFolders(const QString& bucketName, const QStringList& folderNames)
{
	for(int i=0; i<folderNames.size(); i++) {
		ds3_request* request = ds3_init_delete_folder(bucketName.toUtf8().constData(), folderNames[i].toUtf8().constData());
		LOG_INFO("Delete Folder " + bucketName + "/" + folderNames[i]);

		ds3_error* ds3Error = ds3_delete_folder(m_client, request);
		ds3_free_request(request);

		if (ds3Error != NULL) {
			DS3Error error(ds3Error);
			ds3_free_error(ds3Error);
			throw (error);
		}
	}
}

void
Client::BulkGet(const QList<QUrl> urls, const QString& destination)
{
	BulkGetWorkItem* workItem = new BulkGetWorkItem(m_host, urls,
							destination);
	m_bulkWorkItemsLock.lock();
	m_bulkWorkItems[workItem->GetID()] = workItem;
	m_bulkWorkItemsLock.unlock();
	workItem->SetState(Job::QUEUED);
	Job job = workItem->ToJob();
	emit JobProgressUpdate(job);
	run(this, &Client::PrepareBulkGets, workItem);
}

void
Client::BulkPut(const QString& bucketName,
		const QString& prefix,
		const QList<QUrl> urls)
{
	BulkPutWorkItem* workItem = new BulkPutWorkItem(m_host, urls,
							bucketName, prefix);
	m_bulkWorkItemsLock.lock();
	m_bulkWorkItems[workItem->GetID()] = workItem;
	m_bulkWorkItemsLock.unlock();
	workItem->SetState(Job::QUEUED);
	Job job = workItem->ToJob();
	emit JobProgressUpdate(job);
	run(this, &Client::PrepareBulkPuts, workItem);
}

void
Client::GetObject(const QString& bucket,
		  const QString& object,
		  const QString& fileName,
		  uint64_t offset,
		  BulkGetWorkItem* bulkGetWorkItem)
{
	QDir dir(fileName);
	if (object.endsWith("/")) {
		if (!dir.exists()) {
			dir.mkpath(".");
			return;
		}
	} else {
		QDir parentDir(QFileInfo(fileName).absolutePath());
		if (!parentDir.exists()) {
			parentDir.mkpath(".");
		}
	}

	QString jobID = bulkGetWorkItem->GetJobID();
	ds3_request* request = ds3_init_get_object_for_job(bucket.toUtf8().constData(),
							   object.toUtf8().constData(),
							   offset,
							   jobID.toUtf8().constData());
	ds3_error* ds3Error = NULL;
	ObjectWorkItem objWorkItem(bucket, object, fileName, bulkGetWorkItem);
	ClientAndObjectWorkItem caowi;
	caowi.client = this;
	caowi.objectWorkItem = &objWorkItem;
	if (objWorkItem.OpenFile(QIODevice::ReadWrite)) {
		objWorkItem.SeekFile(offset);
		ds3Error = ds3_get_object(m_client, request,
					  &caowi, write_to_file);
	} else {
		LOG_ERROR("ERROR:       GET OBJECT failed, unable to open file "+fileName);
	}

	ds3_free_request(request);

	if (ds3Error != NULL) {
		DS3Error error(ds3Error);
		ds3_free_error(ds3Error);
		throw (error);
	}
}

QFuture<ds3_get_objects_response*>
Client::GetObjects(const QString& bucketName, const QString& name)
{
	QFuture<ds3_get_objects_response*> future = run(this,
						       &Client::DoGetObjects,
						       bucketName,
						       name);
	return future;
}

void
Client::PutObject(const QString& bucket,
		  const QString& object,
		  const QString& fileName,
		  uint64_t offset,
		  uint64_t length,
		  BulkPutWorkItem* workItem)
{
	QString jobID = workItem->GetJobID();
	ds3_request* request = ds3_init_put_object_for_job(bucket.toUtf8().constData(),
							   object.toUtf8().constData(),
							   offset, length,
							   jobID.toUtf8().constData());
	ds3_error* ds3Error = NULL;
	QFileInfo fileInfo(fileName);
	if (fileInfo.isDir()) {
		// "folder" objects don't have a size nor do they have any
		// data associated with them
		ds3Error = ds3_put_object(m_client, request, NULL, NULL);
	} else {
		ObjectWorkItem objWorkItem(bucket, object, fileName, workItem);
		ClientAndObjectWorkItem caowi;
		caowi.client = this;
		caowi.objectWorkItem = &objWorkItem;
		if (objWorkItem.OpenFile(QIODevice::ReadOnly)) {
			objWorkItem.SeekFile(offset);
			ds3Error = ds3_put_object(m_client, request,
						  &caowi, read_from_file);
		} else {
			LOG_ERROR("ERROR:       PUT OBJECT failed, unable to open file "+fileName);
		}
	}
	ds3_free_request(request);

	// TODO Don't rely on WasCanceled to ignore "Request failed: Operation
	// was aborted by an application callback" errors.  It would be nice
	// if the C SDK returned the CURLcode response and we could use that
	// instead.
	if (ds3Error != NULL && !workItem->WasCanceled()) {
		DS3Error error(ds3Error);
		ds3_free_error(ds3Error);
		throw (error);
	}
}

void
Client::CancelBulkJob(QUuid workItemID)
{
	LOG_DEBUG("BULK CANCEL  JOB       "+workItemID.toString());

	m_bulkWorkItemsLock.lock();
	if (m_bulkWorkItems.contains(workItemID)) {
		BulkWorkItem* workItem = m_bulkWorkItems[workItemID];
		Job::State state = workItem->GetState();
		if (state != Job::CANCELING && state != Job::CANCELED &&
		    state != Job::FINISHED) {
			workItem->SetState(Job::CANCELING);
		}
	}
	m_bulkWorkItemsLock.unlock();
}

ds3_get_service_response*
Client::DoGetService()
{
	ds3_request* request = ds3_init_get_service();
	LOG_INFO("BULK GET     BUCKETS   "+m_endpoint);

	ds3_get_service_response *response;
	ds3_error* ds3Error = ds3_get_service(m_client,
					       request,
					       &response);
	ds3_free_request(request);

	if (ds3Error != NULL) {
		DS3Error error(ds3Error);
		ds3_free_error(ds3Error);
		throw (error);
	}

	return response;
}

ds3_get_bucket_response*
Client::DoGetBucket(const QString& bucketName, const QString& prefix,
		    const QString& delimiter, const QString& marker,
		    bool silent)
{
	LOG_DEBUG("GET          Bucket    " + bucketName +
		  ", prefix: " + prefix + ", marker: " + marker);

	ds3_request* request = ds3_init_get_bucket(bucketName.toUtf8().constData());
	QString logFileMsg = "BULK GET     OBJECTS   "+m_endpoint+"/";
	logFileMsg += bucketName;
	QStringList logQueryParams;
	if (!prefix.isEmpty()) {
		ds3_request_set_prefix(request, prefix.toUtf8().constData());
		logQueryParams << "prefix=" + prefix;
	}
	if (!delimiter.isEmpty()) {
		ds3_request_set_delimiter(request, delimiter.toUtf8().constData());
		logQueryParams << "delimiter=" + delimiter;
	}
	if (!marker.isEmpty()) {
		ds3_request_set_marker(request, marker.toUtf8().constData());
		logQueryParams << "marker=" + marker;
	}
	if (MAX_KEYS > 0) {
		ds3_request_set_max_keys(request, MAX_KEYS);
		logQueryParams << "max-keys=" + QString::number(MAX_KEYS);
	}
	if (!logQueryParams.isEmpty()) {
		logFileMsg += "&"+logQueryParams.join("&");
	}

	if (!silent) {
		LOG_INFO(logFileMsg);
	}
	ds3_get_bucket_response* response;
	ds3_error* ds3Error = ds3_get_bucket(m_client,
					     request,
					     &response);
	ds3_free_request(request);

	if (ds3Error != NULL) {
		DS3Error error(ds3Error);
		ds3_free_error(ds3Error);
		throw (error);
	}

	return response;
}

ds3_get_objects_response*
Client::DoGetObjects(const QString& bucketName, const QString& name)
{
	LOG_DEBUG("DoGetObjects - bucket: " + bucketName +
		  ", name: " + name);

	ds3_request* request = ds3_init_get_objects();
	ds3_request_set_bucket_name(request, bucketName.toUtf8().constData());
	QString logMsg = "List Objects (GET " + m_endpoint + "/";
	logMsg += bucketName;
	QStringList logQueryParams;
	if (!name.isEmpty()) {
		ds3_request_set_name(request, name.toUtf8().constData());
		logQueryParams << "name=" + name;
	}
	if (!logQueryParams.isEmpty()) {
		logMsg += "&" + logQueryParams.join("&");
	}
	logMsg += ")";
	LOG_INFO(logMsg);

	ds3_get_objects_response* response;
	ds3_error* ds3Error = ds3_get_objects(m_client,
					     request,
					     &response);
	ds3_free_request(request);

	if (ds3Error != NULL) {
		DS3Error error(ds3Error);
		ds3_free_error(ds3Error);
		throw (error);
	}

	return response;
}

void
Client::PrepareBulkGets(BulkGetWorkItem* workItem)
{
	LOG_DEBUG("PREPARE BULK OBJECT");

	workItem->SetState(Job::PREPARING);
	Job job = workItem->ToJob();
	emit JobProgressUpdate(job);

	workItem->ClearObjMap();

	QString prevBucket;
	QString destination = workItem->GetDestination();

	for (QList<QUrl>::const_iterator& ui(workItem->GetUrlsIterator());
	     ui != workItem->GetUrlsConstEnd();
	     ui++) {
		if (workItem->WasCanceled()) {
			DeleteOrRequeueBulkWorkItem(workItem);
			return;
		}

		DS3URL url(*ui);

		QUrl lastUrl = workItem->GetLastProcessedUrl();
		if (!lastUrl.isEmpty()) {
			QString lastUrlS = lastUrl.toString();
			lastUrlS.replace(QRegularExpression("/$"), "");
			lastUrlS += "/";
			if (url.toString().startsWith(lastUrlS)) {
				// This URL is either the same as or a
				// descendant of the previously processed URL.
				// Since we would have already prepared to
				// transfer all descendents of the previously
				// processed URL, this one can be skipped.
				continue;
			}
		}

		QString bucket = url.GetBucketName();
		if (workItem->GetObjMapSize() >= BULK_PAGE_LIMIT ||
		    (!prevBucket.isEmpty() && prevBucket != bucket)) {
			run(this, &Client::DoBulk, workItem);
			return;
		}
		workItem->SetBucketName(bucket);

		QString fullObjName = url.GetObjectName();
		QString lastPathPart = url.GetLastPathPart();
		QString filePath = QDir::cleanPath(destination + "/" + lastPathPart);
		if (url.IsBucketOrFolder()) {
			QString prefix = fullObjName;
			ds3_get_bucket_response* getBucketRes;
			getBucketRes = workItem->GetGetBucketResponse();
			size_t i = workItem->GetGetBucketResponseIterator();
			do {
				if (workItem->WasCanceled()) {
					DeleteOrRequeueBulkWorkItem(workItem);
					return;
				}
				if (getBucketRes == NULL ||
				    (getBucketRes != NULL && i >= getBucketRes->num_objects)) {
					QString marker;
					if (getBucketRes != NULL) {
						marker = QString::fromUtf8(getBucketRes->next_marker->value);
					}
					getBucketRes = DoGetBucket(bucket, prefix,
								   "", marker);
					i = 0;
				}
				if (getBucketRes->num_objects == 0) {
					workItem->AppendDirsToCreate(filePath);
				}
				for (; i < getBucketRes->num_objects; i++) {
					if (workItem->WasCanceled()) {
						DeleteOrRequeueBulkWorkItem(workItem);
						return;
					}
					if (workItem->GetObjMapSize() >= BULK_PAGE_LIMIT) {
						workItem->SetGetBucketResponse(getBucketRes);
						workItem->SetGetBucketResponseIterator(i);
						run(this, &Client::DoBulk, workItem);
						return;
					}
					ds3_object rawObject = getBucketRes->objects[i];
					QString subFullObjName = QString::fromUtf8(rawObject.name->value);
					QString objNameMinusPrefix = subFullObjName;
					objNameMinusPrefix.replace(QRegularExpression("^" + prefix), "");
					QString subFilePath = QDir::cleanPath(destination + "/" +
									      lastPathPart + "/" +
									      objNameMinusPrefix);
					if (subFullObjName.endsWith("/")) {
						workItem->AppendDirsToCreate(subFilePath);
					} else if (QFile(subFilePath).exists()) {
						LOG_ERROR("ERROR:       "+subFilePath+" already exists. Skipping");
					} else {
						workItem->InsertObjMap(subFullObjName, subFilePath);
					}
				}
			} while (getBucketRes->is_truncated);
			workItem->SetGetBucketResponseIterator(0);
			workItem->SetGetBucketResponse(NULL);
		} else if (QFile(filePath).exists()) {
			LOG_ERROR("ERROR:       "+filePath+" already exists. Skipping");
		} else {
			workItem->InsertObjMap(fullObjName, filePath);
		}

		prevBucket = bucket;
		workItem->SetLastProcessedUrl(*ui);
	}

	if (workItem->GetObjMapSize() > 0) {
		run(this, &Client::DoBulk, workItem);
	} else {
		CreateBulkGetDirs(workItem);
		DeleteOrRequeueBulkWorkItem(workItem);
	}
}

void
Client::PrepareBulkPuts(BulkPutWorkItem* workItem)
{
	LOG_DEBUG("PREPARE BULK PUTS");

	workItem->SetState(Job::PREPARING);
	Job job = workItem->ToJob();
	emit JobProgressUpdate(job);

	workItem->ClearObjMap();
	QString normPrefix = workItem->GetPrefix();
	if (!normPrefix.isEmpty()) {
		normPrefix.replace(QRegularExpression("/$"), "");
		normPrefix += "/";
	}

	for (QList<QUrl>::const_iterator& ui(workItem->GetUrlsIterator());
	     ui != workItem->GetUrlsConstEnd();
	     ui++) {
		if (workItem->WasCanceled()) {
			DeleteOrRequeueBulkWorkItem(workItem);
			return;
		}
		QUrl url(*ui);

		QUrl lastUrl = workItem->GetLastProcessedUrl();
		if (!lastUrl.isEmpty()) {
			QString lastUrlS = lastUrl.toString();
			lastUrlS.replace(QRegularExpression("/$"), "");
			lastUrlS += "/";
			if (url.toString().startsWith(lastUrlS)) {
				// This URL is either the same as or a
				// descendant of the previously processed URL.
				// Since we would have already prepared to
				// transfer all descendents of the previously
				// processed URL, this one can be skipped.
				continue;
			}
		}

		if (workItem->GetObjMapSize() >= BULK_PAGE_LIMIT) {
			run(this, &Client::DoBulk, workItem);
			return;
		}
		QString filePath = url.toLocalFile();
		// filePath could be either /foo or /foo/ if it's a directory.
		// Run it through QDir to normalize it to the former.
		filePath = QDir(filePath).path();
		QFileInfo fileInfo(filePath);
		QString fileName = fileInfo.fileName();
		QString objName = normPrefix + fileName;
		if (fileInfo.isDir()) {
			objName += "/";

			// An existing DirIterator must have been caused by
			// a previous BulkPut "page" that returned early while
			// iterating over files under this URL.  Thus, take
			// over where it left off.
			QDirIterator* di = workItem->GetDirIterator();
			if (di == NULL) {
				di = workItem->GetDirIterator(filePath);
			}
			while (di->hasNext()) {
				if (workItem->WasCanceled()) {
					DeleteOrRequeueBulkWorkItem(workItem);
					return;
				}
				if (workItem->GetObjMapSize() >= BULK_PAGE_LIMIT) {
					run(this, &Client::DoBulk, workItem);
					return;
				}
				QString subFilePath = di->next();
				QFileInfo subFileInfo = di->fileInfo();
				QString subFileName = subFilePath;
				subFileName.replace(QRegularExpression("^" + filePath + "/"), "");
				QString subObjName = objName + subFileName;
				if (subFileInfo.isDir()) {
					subObjName += "/";
				}
				workItem->InsertObjMap(subObjName, subFilePath);
			}
			workItem->DeleteDirIterator();
		}
		workItem->InsertObjMap(objName, filePath);
		workItem->SetLastProcessedUrl(*ui);
	}

	if (workItem->GetObjMapSize() > 0) {
		run(this, &Client::DoBulk, workItem);
	}
}

void
Client::DoBulk(BulkWorkItem* workItem)
{
	LOG_DEBUG("DO BULK");

	workItem->SetState(Job::INPROGRESS);
	workItem->SetTransferStartIfNull();
	Job job = workItem->ToJob();
	emit JobProgressUpdate(job);

	uint64_t numFiles = workItem->GetObjMapSize();
	ds3_bulk_object_list *bulkObjList = ds3_init_bulk_object_list(numFiles);

	bool isGet = workItem->GetType() == Job::GET;

	QHash<QString, QString>::const_iterator hi;
	size_t i = 0;
	for (hi = workItem->GetObjMapConstBegin();
	     hi != workItem->GetObjMapConstEnd();
	     hi++) {
		ds3_bulk_object* bulkObj = &bulkObjList->list[i];
		QString objName = hi.key();
		QString filePath = hi.value();
		QFileInfo fileInfo(filePath);
		bulkObj->name = ds3_str_init(objName.toUtf8().constData());
		if (!isGet) {
			uint64_t fileSize = 0;
			if (!fileInfo.isDir()) {
				fileSize = GetFileSize(filePath);
			}
			bulkObj->length = fileSize;
			bulkObj->offset = 0;
		}
		i++;
	}

	const QString& bucketName = workItem->GetBucketName();
	ds3_request* request;
	if (isGet) {
		request = ds3_init_get_bulk(bucketName.toUtf8().constData(), bulkObjList, NONE);
	} else {
		request = ds3_init_put_bulk(bucketName.toUtf8().constData(), bulkObjList);
	}
	ds3_bulk_response *response = NULL;
	ds3_error* ds3Error = ds3_bulk(m_client, request, &response);
	ds3_free_request(request);
	ds3_free_bulk_object_list(bulkObjList);
	workItem->SetResponse(response);
	workItem->SetNumChunksProcessed(0);

	if (ds3Error != NULL) {
		DS3Error error(ds3Error);
		ds3_free_error(ds3Error);
		if (isGet) {
			throw (error);
		} else {
			QString errorFileMsg = "ERROR:       Uploading objects to server, ";

			if (error.GetStatusCode() == 409) {
				QString body = error.GetErrorBody();
				QRegularExpression rx(", ([^\\)]+)\\) already exists");
				QRegularExpressionMatch match = rx.match(body);
				if (match.hasMatch()) {
					errorFileMsg += match.captured(1)+ "already exists"\
					                "and objects cannot be replaced";
				} else {
					errorFileMsg += "one or more of the objects "\
					                "already exists and objects "\
					                "cannot be replaced";
				}
			} else {
				errorFileMsg += error.ToString();
			}
			errorFileMsg += ".  Canceling job.";
			LOG_ERROR(errorFileMsg);
			workItem->SetResponse(NULL);
			DeleteOrRequeueBulkWorkItem(workItem);
			return;
		}
	}

	if (isGet) {
		CreateBulkGetDirs(static_cast<BulkGetWorkItem*>(workItem));
	}

	if (response == NULL || (response != NULL && response->list_size == 0)) {
		DeleteOrRequeueBulkWorkItem(workItem);
		return;
	}

	ProcessJobChunk(workItem);
	LOG_INFO("BULK JOB     Complete");
}

void
Client::CreateBulkGetDirs(BulkGetWorkItem* workItem)
{
	for (int i = 0; i < workItem->GetDirsToCreateSize(); i++) {
		QDir dir(workItem->GetDirsToCreateAt(i));
		if (!dir.exists()) {
			dir.mkpath(".");
		}
	}
	workItem->ClearDirsToCreate();
}

void
Client::ProcessJobChunk(BulkWorkItem* workItem)
{
	LOG_DEBUG("PROCESS GET  JOB CHUNK");

	// If the get available chunks response doesn't include any objects, it
	// means the server isn't ready yet (e.g. it could still be transferring
	// objects off tape and into cache).  In this situation, we have to
	// wait and try again.
	ds3_get_available_chunks_response* chunksResponse = NULL;
	size_t numChunks = 0;
	while (numChunks == 0) {
		if (workItem->WasCanceled()) {
			ds3_free_available_chunks_response(chunksResponse);
			DeleteOrRequeueBulkWorkItem(workItem);
			return;
		}
		uint64_t retryAfter = 60;
		QString errMsg;
		try {
			chunksResponse = GetAvailableJobChunks(workItem);
			numChunks = chunksResponse->object_list->list_size;
			retryAfter = chunksResponse->retry_after;
		}
		catch (DS3Error& e) {
			LOG_ERROR("ERROR:       GET JOB CHUNKS failed, "+e.ToString());

		}
		if (numChunks == 0) {
			ds3_free_available_chunks_response(chunksResponse);
			LOG_INFO("BULK GET     JOB CHUNK Not ready. Sleeping for " +
				  QString::number(retryAfter) + " seconds.");
			QThread::sleep(retryAfter);
		}
	}

	QString bucketName = workItem->GetBucketName();
	bool isGet = workItem->GetType() == Job::GET;
	QString op = isGet ? "GET" : "PUT";
	// TODO PUT multiple objects at once.  Start the next chunk as soon
	//      as the number of objects left to put is less than the size
	//      of the "put objects" thread pool.
	ds3_bulk_response* bulkResponse = chunksResponse->object_list;
	for (size_t chunk = 0; chunk < numChunks; chunk++) {
		ds3_bulk_object_list* list = bulkResponse->list[chunk];
		for (uint64_t i = 0; i < list->size;  i++) {
			if (workItem->WasCanceled()) {
				DeleteOrRequeueBulkWorkItem(workItem);
				return;
			}
			ds3_bulk_object* bulkObj = &(list->list[i]);
			QString objName = QString::fromUtf8(bulkObj->name->value);
			QString filePath = workItem->GetObjMapValue(objName);
			uint64_t offset = bulkObj->offset;
			try {
				if (isGet) {
					Client::GetObject(bucketName, objName,
							  filePath, offset,
							  static_cast<BulkGetWorkItem*>(workItem));
					LOG_FILE(QString("     GET     OBJECT    ")+"/"+bucketName+"/"+objName+"->"+filePath);
				} else {
					uint64_t length = bulkObj->length;
					Client::PutObject(bucketName, objName,
							  filePath, offset,
							  length,
							  static_cast<BulkPutWorkItem*>(workItem));
					LOG_FILE(QString("     PUT     OBJECT    ")+filePath+"->"+"/"+bucketName+"/"+objName);
				}
			}
			catch (DS3Error& e) {
				LOG_ERROR("ERROR:       " + op + " OBJECT failed, "+objName+
					  "\" - "+e.ToString());
			}
		}
		workItem->IncNumChunksProcessed();
	}
	ds3_free_available_chunks_response(chunksResponse);

	if (workItem->IsPageFinished()) {
		DeleteOrRequeueBulkWorkItem(workItem);
	} else {
		run(this, &Client::ProcessJobChunk, workItem);
	}
}

ds3_get_available_chunks_response*
Client::GetAvailableJobChunks(BulkWorkItem* workItem)
{
	ds3_bulk_response *response = workItem->GetResponse();
	ds3_request* request = ds3_init_get_available_chunks(response->job_id->value);
	ds3_get_available_chunks_response* chunkResponse;
	ds3_error* ds3Error = ds3_get_available_chunks(m_client, request, &chunkResponse);
	ds3_free_request(request);

	if (ds3Error != NULL) {
		DS3Error error(ds3Error);
		ds3_free_error(ds3Error);
		throw (error);
	}

	return chunkResponse;
}

void
Client::DeleteOrRequeueBulkWorkItem(BulkWorkItem* workItem)
{
	if (workItem->WasCanceled()) {
		LOG_INFO("BULK GET     JOB       Canceled");
		workItem->SetState(Job::CANCELED);
		Job job = workItem->ToJob();
		emit JobProgressUpdate(job);
		DeleteBulkWorkItem(workItem);
	} else if (workItem->IsPageFinished()) {
		if (workItem->IsFinished()) {
			LOG_DEBUG("Finished with bulk work item.  Deleting it.");
			workItem->SetState(Job::FINISHED);
			Job job = workItem->ToJob();
			emit JobProgressUpdate(job);
			DeleteBulkWorkItem(workItem);
		} else {
			LOG_DEBUG("More bulk pages to go.  Starting PrepareBulk{Gets,Puts} again.");
			if (workItem->GetType() == Job::GET) {
				run(this,
				    &Client::PrepareBulkGets,
				    static_cast<BulkGetWorkItem*>(workItem));
			} else {
				run(this,
				    &Client::PrepareBulkPuts,
				    static_cast<BulkPutWorkItem*>(workItem));
			}
		}
	} else {
		LOG_DEBUG("Page not finished. num chunks processed: " +
			  QString::number(workItem->GetNumChunksProcessed()));
	}
}

void
Client::DeleteBulkWorkItem(BulkWorkItem* workItem)
{
	m_bulkWorkItemsLock.lock();
	m_bulkWorkItems.remove(workItem->GetID());
	delete workItem;
	m_bulkWorkItemsLock.unlock();
}

qint64
Client::GetFileSize(const QString& path)
{
	qint64 size = 0;
#ifdef Q_OS_WIN
	// There's a bug with QFileInfo::size() where it will report the size
	// of a shortcut's target instead of the actual shortcut.
	// See https://bugreports.qt.io/browse/QTBUG-24831
	WIN32_FILE_ATTRIBUTE_DATA data;
	QString nativePath = QDir::toNativeSeparators(path);
	bool ok = GetFileAttributesEx((wchar_t*)nativePath.utf16(),
				      GetFileExInfoStandard, &data);
	if (ok) {
		size = data.nFileSizeHigh;
		size <<= 32;
		size += data.nFileSizeLow;
	} else {
		LOG_ERROR("ERROR:       GET FILE SIZE failed for "+nativePath);
	}
#else
	size = QFileInfo(path).size();
#endif
	return size;
}

static size_t
read_from_file(void* buffer, size_t size, size_t count, void* user_data)
{
	ClientAndObjectWorkItem* caowi = static_cast<ClientAndObjectWorkItem*>(user_data);
	Client* client = caowi->client;
	ObjectWorkItem* workItem = caowi->objectWorkItem;
	return client->ReadFile(workItem, (char*)buffer, size, count);
}

size_t
Client::ReadFile(ObjectWorkItem* workItem, char* buffer,
		 size_t size, size_t count)
{
	BulkWorkItem* bulkWorkItem = workItem->GetBulkWorkItem();
	if (bulkWorkItem != NULL && bulkWorkItem->WasCanceled()) {
		return DS3_READFUNC_ABORT;
	}

	size_t bytesRead = workItem->ReadFile(buffer, size, count);
	if (bulkWorkItem != NULL && bulkWorkItem->IsJobUpdateReady()) {
		Job job = bulkWorkItem->ToJob();
		emit JobProgressUpdate(job);
	}
	return bytesRead;
}

static size_t
write_to_file(void* buffer, size_t size, size_t count, void* user_data)
{
	ClientAndObjectWorkItem* caowi = static_cast<ClientAndObjectWorkItem*>(user_data);
	Client* client = caowi->client;
	ObjectWorkItem* workItem = caowi->objectWorkItem;
	return client->WriteFile(workItem, (char*)buffer, size, count);
}

size_t
Client::WriteFile(ObjectWorkItem* workItem, char* buffer,
		  size_t size, size_t count)
{
	BulkWorkItem* bulkWorkItem = workItem->GetBulkWorkItem();
	if (bulkWorkItem != NULL && bulkWorkItem->WasCanceled()) {
		return 0;
	}

	size_t bytesWritten = workItem->WriteFile(buffer, size, count);
	if (bulkWorkItem != NULL && bulkWorkItem->IsJobUpdateReady()) {
		Job job = bulkWorkItem->ToJob();
		emit JobProgressUpdate(job);
	}
	return bytesWritten;
}
