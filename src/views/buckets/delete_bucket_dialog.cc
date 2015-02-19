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

#include <ds3.h>

#include "lib/client.h"
#include "lib/logger.h"
#include "views/buckets/delete_bucket_dialog.h"

DeleteBucketDialog::DeleteBucketDialog(Client* client,
				       const QString& bucketName,
				       QWidget* parent)
	: DS3DeleteDialog(client, bucketName, false, parent)
{
	setWindowTitle("Delete Bucket");
	m_warning->setText("This action will delete the \"" + bucketName +
			   "\". Are you sure you wish to continue?");
}

bool
DeleteBucketDialog::Delete()
{
	try {
		m_client->DeleteBucket(m_bucketName);
	}
	catch (DS3Error& e) {
		QString body;
		QString msg;
		switch (e.GetStatusCode()) {
		case 403:
			body = e.GetErrorBody();
			if (body.contains("spectra-", Qt::CaseInsensitive)) {
				msg = "Buckets that start with \"spectra-\" " \
				      "are reserved and cannot be deleted";
			}
			break;
		case 409:
			body = e.GetErrorBody();
			if (body.contains("BUCKET_NOT_EMPTY")) {
				msg = "Only empty buckets can be deleted";
			}
			break;
		}
		if (msg.isEmpty()) {
			msg = e.ToString();
		}
		m_baseErrorLabel->setText(msg);
		m_form->addWidget(m_baseErrorLabel, 0, 0, 1, 3);
		LOG_ERROR("Error deleting bucket - " + msg);
		return false;
	}

	return true;
}
