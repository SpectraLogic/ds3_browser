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

#include "models/session.h"

#include "views/session_dialog.h"

SessionDialog::SessionDialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle("New Spectra Logic DS3 Session");

	m_form = new QFormLayout;

	m_hostLineEdit = new QLineEdit;
	m_hostLineEdit->setToolTip("The name or IP address of the DS3 " \
				   "system's data interface");
	m_hostLabel = new QLabel("BlackPearl Name or IP Address");
	m_form->addRow(m_hostLabel, m_hostLineEdit);

	m_portComboBox = new QComboBox();
	m_portComboBox->addItem("80");
	m_portComboBox->addItem("8080");
	m_portComboBox->setToolTip("The port that the DS3 system's DS3 " \
				   "service is running on (usually 80)");
	m_form->addRow("BlackPearl DS3 Port", m_portComboBox);

	m_accessIdLineEdit = new QLineEdit;
	m_accessIdLabel = new QLabel("S3 Access ID");
	m_form->addRow(m_accessIdLabel, m_accessIdLineEdit);

	m_secretKeyLineEdit = new QLineEdit;
	m_secretKeyLabel = new QLabel("S3 Secret Key");
	m_form->addRow(m_secretKeyLabel, m_secretKeyLineEdit);

	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
					   | QDialogButtonBox::Cancel);

	connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(Accept()));
	connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(Reject()));

	m_layout = new QVBoxLayout;
	m_layout->addLayout(m_form);
	m_layout->addWidget(m_buttonBox);

	setLayout(m_layout);

	m_session = new Session;
}

SessionDialog::~SessionDialog()
{
	delete m_session;
}

void
SessionDialog::Accept()
{
	bool valid = ValidateLineEditNotEmpty(m_hostLabel, m_hostLineEdit);
 	valid &= ValidateLineEditNotEmpty(m_accessIdLabel, m_accessIdLineEdit);
	valid &= ValidateLineEditNotEmpty(m_secretKeyLabel, m_secretKeyLineEdit);
	if (!valid) {
		return;
	}

	UpdateSession();
	accept();
}

bool
SessionDialog::ValidateLineEditNotEmpty(QLabel* label, QLineEdit* lineEdit)
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
SessionDialog::Reject()
{
	reject();
}

void
SessionDialog::UpdateSession()
{
	m_session->SetHost(m_hostLineEdit->text().trimmed().toUtf8().constData());
	m_session->SetPort(m_portComboBox->currentText().trimmed().toUtf8().constData());
	m_session->SetAccessId(m_accessIdLineEdit->text().trimmed().toUtf8().constData());
	m_session->SetSecretKey(m_secretKeyLineEdit->text().trimmed().toUtf8().constData());
}
