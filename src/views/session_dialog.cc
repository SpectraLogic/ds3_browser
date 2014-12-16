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
	  m_form(new QGridLayout),
	  m_hostLineEdit(new QLineEdit),
	  m_portComboBox(new QComboBox),
	  m_proxyLineEdit(new QLineEdit),
	  m_accessIdLineEdit(new QLineEdit),
	  m_secretKeyLineEdit(new QLineEdit),
	  m_session(new Session)
{
	setWindowTitle("New Spectra Logic DS3 Session");

	m_hostLineEdit->setFixedWidth(150);
	m_hostLineEdit->setToolTip("The name or IP address of the DS3 " \
				   "system's data interface");
	m_hostLabel = new QLabel("BlackPearl Name or IP Address");
	m_hostErrorLabel = new QLabel;
	m_hostErrorLabel->setStyleSheet("QLabel { color: red; }");
	m_form->addWidget(m_hostLabel, 1, 0);
	m_form->addWidget(m_hostLineEdit, 1, 1);
	m_form->addWidget(m_hostErrorLabel, 1, 2);

	m_portLabel = new QLabel("BlackPearl DS3 Port");
	m_portComboBox->addItem("80");
	m_portComboBox->addItem("8080");
	m_portComboBox->setToolTip("The port that the DS3 system's DS3 " \
				   "service is running on (usually 80)");
	m_form->addWidget(m_portLabel, 2, 0);
	m_form->addWidget(m_portComboBox, 2, 1);

	m_proxyLineEdit->setToolTip("An optional server to proxy requests");
	m_proxyLabel = new QLabel("Proxy Server");
	m_form->addWidget(m_proxyLabel, 3, 0);
	m_form->addWidget(m_proxyLineEdit, 3, 1);

	m_accessIdLabel = new QLabel("S3 Access ID");
	m_accessIdErrorLabel = new QLabel;
	m_accessIdErrorLabel->setStyleSheet("QLabel { color: red; }");
	m_form->addWidget(m_accessIdLabel, 4, 0);
	m_form->addWidget(m_accessIdLineEdit, 4, 1);
	m_form->addWidget(m_accessIdErrorLabel, 4, 2);

	m_secretKeyLabel = new QLabel("S3 Secret Key");
	m_secretKeyErrorLabel = new QLabel;
	m_secretKeyErrorLabel->setStyleSheet("QLabel { color: red; }");
	m_form->addWidget(m_secretKeyLabel, 5, 0);
	m_form->addWidget(m_secretKeyLineEdit, 5, 1);
	m_form->addWidget(m_secretKeyErrorLabel, 5, 2);

	m_saveSessionCheckBox = new QCheckBox("Save Session");
	m_form->addWidget(m_saveSessionCheckBox, 6, 1);

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
	bool valid = ValidateLineEditNotEmpty(m_hostLabel,
					      m_hostLineEdit,
					      m_hostErrorLabel);
 	valid &= ValidateLineEditNotEmpty(m_accessIdLabel,
					  m_accessIdLineEdit,
					  m_accessIdErrorLabel);
	valid &= ValidateLineEditNotEmpty(m_secretKeyLabel,
					  m_secretKeyLineEdit,
					  m_secretKeyErrorLabel);
	if (!valid) {
		return;
	}

	UpdateSession();
	SaveSession();
	accept();
}

bool
SessionDialog::ValidateLineEditNotEmpty(QLabel* label,
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
		m_session->SetProxy(settings.value("proxy").toString());
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
	m_proxyLineEdit->setText(m_session->GetProxy());
	m_accessIdLineEdit->setText(m_session->GetAccessId());
	m_secretKeyLineEdit->setText(m_session->GetSecretKey());
}

void
SessionDialog::UpdateSession()
{
	m_session->SetHost(m_hostLineEdit->text().trimmed().toUtf8().constData());
	m_session->SetPort(m_portComboBox->currentText().trimmed().toUtf8().constData());
	m_session->SetProxy(m_proxyLineEdit->text().trimmed().toUtf8().constData());
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
		settings.setValue("proxy", m_session->GetProxy());
		settings.setValue("port", m_session->GetPort());
		settings.setValue("withCertificateVerification", m_session->GetWithCertificateVerification());
		settings.setValue("accessID", m_session->GetAccessId());
		settings.setValue("secretKey", m_session->GetSecretKey());
	} else {
		settings.remove("");
	}
	settings.endArray();
}
