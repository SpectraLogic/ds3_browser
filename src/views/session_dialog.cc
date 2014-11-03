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

#include <Qt> // Needed for QComboBox's use Qt::MatchFlags
#include <QSettings>

#include "models/session.h"

#include "views/session_dialog.h"

SessionDialog::SessionDialog(QWidget* parent)
	: QDialog(parent),
	  m_layout(new QVBoxLayout),
	  m_form(new QFormLayout),
	  m_hostLineEdit(new QLineEdit),
	  m_portComboBox(new QComboBox),
	  m_accessIdLineEdit(new QLineEdit),
	  m_secretKeyLineEdit(new QLineEdit),
	  m_session(new Session)
{
	setWindowTitle("New Spectra Logic DS3 Session");

	m_hostLineEdit->setToolTip("The name or IP address of the DS3 " \
				   "system's data interface");
	m_hostLabel = new QLabel("BlackPearl Name or IP Address");
	m_form->addRow(m_hostLabel, m_hostLineEdit);

	m_portComboBox->addItem("80");
	m_portComboBox->addItem("8080");
	m_portComboBox->setToolTip("The port that the DS3 system's DS3 " \
				   "service is running on (usually 80)");
	m_form->addRow("BlackPearl DS3 Port", m_portComboBox);

	m_accessIdLabel = new QLabel("S3 Access ID");
	m_form->addRow(m_accessIdLabel, m_accessIdLineEdit);

	m_secretKeyLabel = new QLabel("S3 Secret Key");
	m_form->addRow(m_secretKeyLabel, m_secretKeyLineEdit);

	m_saveSessionCheckBox = new QCheckBox("Save Session");
	m_form->addRow("", m_saveSessionCheckBox);

	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
					   | QDialogButtonBox::Cancel);

	connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(Accept()));
	connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(Reject()));

	m_layout->addLayout(m_form);
	m_layout->addWidget(m_buttonBox);

	setLayout(m_layout);

	LoadSession();
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
	SaveSession();
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
SessionDialog::LoadSession()
{
	QSettings settings;
	int size = settings.beginReadArray("sessions");
	if (size > 0) {
		// This only supports saving a loading/saving a single
		// session.  If/when this is updated to support saving
		// multiple sessions, this should load the last-used
		// session.
		settings.setArrayIndex(0);
		m_session->SetHost(settings.value("host").toString());
		m_session->SetProtocol(settings.value("protocol").toInt());
		m_session->SetPort(settings.value("port").toString());
		m_session->SetWithCertificateVerification(settings.value("withCertificateVerification").toBool());
		m_session->SetAccessId(settings.value("accessID").toString());
		m_session->SetSecretKey(settings.value("secretKey").toString());

		m_saveSessionCheckBox->setChecked(true);
	}
	settings.endArray();

	m_hostLineEdit->setText(m_session->GetHost());
	int portIndex = m_portComboBox->findText(m_session->GetPort());
	if (portIndex != -1) {
		m_portComboBox->setCurrentIndex(portIndex);
	}
	m_accessIdLineEdit->setText(m_session->GetAccessId());
	m_secretKeyLineEdit->setText(m_session->GetSecretKey());
}

void
SessionDialog::UpdateSession()
{
	m_session->SetHost(m_hostLineEdit->text().trimmed().toUtf8().constData());
	m_session->SetPort(m_portComboBox->currentText().trimmed().toUtf8().constData());
	m_session->SetAccessId(m_accessIdLineEdit->text().trimmed().toUtf8().constData());
	m_session->SetSecretKey(m_secretKeyLineEdit->text().trimmed().toUtf8().constData());
}

void
SessionDialog::SaveSession()
{
	// TODO If/when this is updated to support saving multiple sessions,
	//      this should be updated to also save a last-used setting.
	QSettings settings;
	settings.beginWriteArray("sessions");
	if (m_saveSessionCheckBox->isChecked()) {
		settings.setArrayIndex(0);
		settings.setValue("host", m_session->GetHost());
		settings.setValue("protocol", m_session->GetProtocol());
		settings.setValue("port", m_session->GetPort());
		settings.setValue("withCertificateVerification", m_session->GetWithCertificateVerification());
		settings.setValue("accessID", m_session->GetAccessId());
		settings.setValue("secretKey", m_session->GetSecretKey());
	} else {
		settings.remove("");
	}
	settings.endArray();
}
