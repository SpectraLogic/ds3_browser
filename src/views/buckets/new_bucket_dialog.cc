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
#include "views/buckets/new_bucket_dialog.h"

NewBucketDialog::NewBucketDialog(Client* client, QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle("New Bucket");

	m_client = client;

	m_form = new QFormLayout;

	m_bucketLineEdit = new QLineEdit;
	m_bucketLineEdit->setToolTip("The name of the Bucket to create");
	m_bucketLabel = new QLabel("Bucket Name");
	m_form->addRow(m_bucketLabel, m_bucketLineEdit);

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
	if (!ValidateLineEditNotEmpty(m_bucketLabel, m_bucketLineEdit)) {
		return;
	}

	UpdateBucket();
	CreateBucket();
	accept();
}

bool
NewBucketDialog::ValidateLineEditNotEmpty(QLabel* label, QLineEdit* lineEdit)
{
	bool valid = true;
	if (lineEdit->text().trimmed().isEmpty()) {
		label->setStyleSheet("QLabel { color: red; }");
		valid = false;
	} else {
		label->setStyleSheet("");
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
	m_client->CreateBucket(m_bucket.toUtf8().constData());
}
