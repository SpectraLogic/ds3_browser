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

#include "lib/client.h"
#include "lib/logger.h"
#include "lib/errors/ds3_error.h"
#include "views/buckets/new_bucket_dialog.h"

NewBucketDialog::NewBucketDialog(Client* client, QWidget* parent)
	: QDialog(parent),
	  m_baseErrorLabel(NULL)
{
	setWindowTitle("New Bucket");

	m_client = client;

	m_form = new QGridLayout;

	m_bucketLineEdit = new QLineEdit;
	m_bucketLineEdit->setToolTip("The name of the Bucket to create");
	m_bucketLineEdit->setFixedWidth(150);
	m_bucketLabel = new QLabel("Bucket Name");
	m_bucketErrorLabel = new QLabel;
	m_bucketErrorLabel->setStyleSheet("QLabel { color: red; }");
	m_form->addWidget(m_bucketLabel, 1, 0);
	m_form->addWidget(m_bucketLineEdit, 1, 1);
	m_form->addWidget(m_bucketErrorLabel, 1, 2);

	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
					   | QDialogButtonBox::Cancel);

	connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(Accept()));
	connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(Reject()));

	m_layout = new QVBoxLayout;
	m_layout->addLayout(m_form);
	m_layout->addWidget(m_buttonBox);

	setLayout(m_layout);
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
		QString msg;
		switch (e.GetStatusCode()) {
		case 400:
			msg = "Bucket \"" + m_bucket + "\" is invalid";
			m_bucketLabel->setStyleSheet("QLabel { color: red; }");
			m_bucketErrorLabel->setText("is invalid");
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

bool
NewBucketDialog::ValidateLineEditNotEmpty(QLabel* label,
					  QLineEdit* lineEdit,
					  QLabel* errorLabel)
{
	bool valid = true;
	if (lineEdit->text().trimmed().isEmpty()) {
		label->setStyleSheet("QLabel { color: red; }");
		errorLabel->setText("cannot be blank");
		valid = false;
	} else {
		label->setStyleSheet("");
		errorLabel->setText("");
	}
	return valid;
}

void
NewBucketDialog::Reject()
{
	reject();
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
