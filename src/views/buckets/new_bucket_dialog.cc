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

#include "lib/client.h"
#include "lib/logger.h"
#include "lib/errors/ds3_error.h"
#include "views/buckets/new_bucket_dialog.h"

NewBucketDialog::NewBucketDialog(Client* client, QWidget* parent)
	: Dialog(parent),
	  m_client(client)
{
	setWindowTitle("New Bucket");

	m_bucketLineEdit = new QLineEdit;
	m_bucketLineEdit->setToolTip("The name of the Bucket to create");
	m_bucketLineEdit->setFixedWidth(150);
	m_bucketLabel = new QLabel("Bucket Name");
	m_bucketErrorLabel = new QLabel;
	m_bucketErrorLabel->setStyleSheet("QLabel { color: red; }");
	m_form->addWidget(m_bucketLabel, 1, 0);
	m_form->addWidget(m_bucketLineEdit, 1, 1);
	m_form->addWidget(m_bucketErrorLabel, 1, 2);

	m_bucketLineEdit->setFocus();
}

void
NewBucketDialog::Accept()
{
	if (m_baseErrorLabel == NULL) {
		m_baseErrorLabel = new QLabel;
		m_baseErrorLabel->setStyleSheet("QLabel { color: red; }");
	} else {
		m_baseErrorLabel->setText("");
		m_form->removeWidget(m_baseErrorLabel);
	}
	if (!ValidateLineEditNotEmpty(m_bucketLabel,
				      m_bucketLineEdit,
				      m_bucketErrorLabel)) {
		return;
	}

	UpdateBucket();
	try {
		CreateBucket();
	}
	catch (DS3Error& e) {
		QString body;
		QString msg;
		switch (e.GetStatusCode()) {
		case 400:
			msg = "Bucket \"" + m_bucket + "\" is invalid";
			m_bucketLabel->setStyleSheet("QLabel { color: red; }");
			m_bucketErrorLabel->setText("is invalid");
			break;
		case 403:
			body = e.GetErrorBody();
			// TODO DS3Error should properly XML decode the body
			if (body.contains("<Code>Forbidden</Code>") &&
			    body.contains("spectra-", Qt::CaseInsensitive) &&
			    body.contains("reserved", Qt::CaseInsensitive)) {
				m_bucketLabel->setStyleSheet("QLabel { color: red; }");
				msg = "Bucket names that start with \"spectra-\" are reserved";
				m_bucketErrorLabel->setText(msg);
			}
			break;
		case 409:
			msg = "Bucket \"" + m_bucket + "\" already exists";
			m_bucketLabel->setStyleSheet("QLabel { color: red; }");
			m_bucketErrorLabel->setText("already exists");
			break;
		default:
			msg = e.ToString();
			m_baseErrorLabel->setText(msg);
			m_form->addWidget(m_baseErrorLabel, 0, 0, 1, 3);
		}
		LOG_ERROR("Error creating bucket - " + msg);
		return;
	}
	accept();
}

void
NewBucketDialog::UpdateBucket()
{
	m_bucket = m_bucketLineEdit->text().trimmed();
}

void
NewBucketDialog::CreateBucket()
{
	m_client->CreateBucket(m_bucket);
}
