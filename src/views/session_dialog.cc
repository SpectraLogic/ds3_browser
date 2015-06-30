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

#include <Qt> // Needed for QComboBox's use Qt::MatchFlags
#include <QApplication>
#include <QPixmap>
#include <QSettings>

#include "global.h"
#include "lib/client.h"
#include "lib/logger.h"
#include "lib/watchers/get_bucket_watcher.h"
#include "views/session_dialog.h"

SessionDialog::SessionDialog(QWidget* parent)
	: Dialog(parent),
	  m_headerWidget(new QWidget),
	  m_header(new QGridLayout),
	  m_hostLineEdit(new QLineEdit),
	  m_portComboBox(new QComboBox),
	  m_proxyLineEdit(new QLineEdit),
	  m_accessIdLineEdit(new QLineEdit),
	  m_secretKeyLineEdit(new QLineEdit),
	  m_client(NULL),
	  m_watcher(NULL)
{
	setWindowTitle("New " + APP_NAME + " Session");

	m_headerWidget->setLayout(m_header);
	m_layout->insertWidget(0, m_headerWidget);

	m_header->setSpacing(0);
	m_header->setMargin(0);
	m_header->setContentsMargins(0, 0, 0, 0);

	QString logoStyle = "QLabel { background-color: black; padding: 10px; }";

	m_bpLogoLabel = new QLabel;
	QPixmap bpLogoPixmap(":/resources/logos/black_pearl.png");
	m_bpLogoLabel->setPixmap(bpLogoPixmap);
	m_bpLogoLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	m_bpLogoLabel->setStyleSheet(logoStyle);
	m_header->addWidget(m_bpLogoLabel, 0, 0);

	m_spectraLogoLabel = new QLabel;
	QPixmap spectraLogoPixmap(":/resources/logos/spectra.png");
	m_spectraLogoLabel->setPixmap(spectraLogoPixmap);
	m_spectraLogoLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	m_spectraLogoLabel->setStyleSheet(logoStyle);
	m_header->addWidget(m_spectraLogoLabel, 0, 1);

	QString tip = "The DNS name or IP address of the " \
		      "BlackPearl's network data interface (not " \
		      "the network management interface)";
	m_hostLineEdit->setFixedWidth(150);
	m_hostLineEdit->setToolTip(tip);
	m_hostLabel = new QLabel("BlackPearl Data Interface");
	m_hostLabel->setToolTip(tip);
	m_hostErrorLabel = new QLabel;
	m_hostErrorLabel->setStyleSheet("QLabel { color: red; }");
	m_form->addWidget(m_hostLabel, 1, 0);
	m_form->addWidget(m_hostLineEdit, 1, 1);
	m_form->addWidget(m_hostErrorLabel, 1, 2);

	tip = "The HTTP port that the BlackPearl's DS3 " \
	      "server is running on.  The default is 80";
	m_portLabel = new QLabel("BlackPearl DS3 Port");
	m_portLabel->setToolTip(tip);
	m_portComboBox->addItem("80");
	m_portComboBox->addItem("8080");
	m_portComboBox->setToolTip(tip);
	m_form->addWidget(m_portLabel, 2, 0);
	m_form->addWidget(m_portComboBox, 2, 1);

	tip = "An optional server to proxy requests";
	m_proxyLabel = new QLabel("Proxy Server");
	m_proxyLabel->setToolTip(tip);
	m_proxyLineEdit->setToolTip(tip);
	m_form->addWidget(m_proxyLabel, 3, 0);
	m_form->addWidget(m_proxyLineEdit, 3, 1);

	tip = "The user's S3 Access ID.  This is available via the " \
	      "BlackPearl user interface";
	m_accessIdLabel = new QLabel("S3 Access ID");
	m_accessIdLabel->setToolTip(tip);
	m_accessIdLineEdit->setToolTip(tip);
	m_accessIdErrorLabel = new QLabel;
	m_accessIdErrorLabel->setStyleSheet("QLabel { color: red; }");
	m_form->addWidget(m_accessIdLabel, 4, 0);
	m_form->addWidget(m_accessIdLineEdit, 4, 1);
	m_form->addWidget(m_accessIdErrorLabel, 4, 2);

	tip = "The user's S3 Secret Key.  This is available via the " \
	      "BlackPearl user interface";
	m_secretKeyLabel = new QLabel("S3 Secret Key");
	m_secretKeyLabel->setToolTip(tip);
	m_secretKeyLineEdit->setToolTip(tip);
	m_secretKeyErrorLabel = new QLabel;
	m_secretKeyErrorLabel->setStyleSheet("QLabel { color: red; }");
	m_form->addWidget(m_secretKeyLabel, 5, 0);
	m_form->addWidget(m_secretKeyLineEdit, 5, 1);
	m_form->addWidget(m_secretKeyErrorLabel, 5, 2);

	m_saveSessionCheckBox = new QCheckBox("Save Session");
	m_form->addWidget(m_saveSessionCheckBox, 6, 1);

	m_form->addWidget(m_buttonBox, 7, 1, 1, 2);

	LoadSession();
}

SessionDialog::~SessionDialog()
{
	if (m_client != NULL) {
		delete m_client;
	}
	if (m_watcher != NULL) {
		delete m_watcher;
	}
}

void
SessionDialog::Accept()
{
	if (m_baseErrorLabel == NULL) {
		m_baseErrorLabel = new QLabel;
		m_baseErrorLabel->setStyleSheet("QLabel { color: red; }");
	} else {
		m_baseErrorLabel->setText("");
		m_form->removeWidget(m_baseErrorLabel);
	}
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
	Authenticate();
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
		m_session.SetHost(settings.value("host").toString());
		m_session.SetProtocol(settings.value("protocol").toInt());
		m_session.SetPort(settings.value("port").toString());
		m_session.SetProxy(settings.value("proxy").toString());
		m_session.SetWithCertificateVerification(settings.value("withCertificateVerification").toBool());
		m_session.SetAccessId(settings.value("accessID").toString());
		m_session.SetSecretKey(settings.value("secretKey").toString());

		m_saveSessionCheckBox->setChecked(true);
	}
	settings.endArray();

	m_hostLineEdit->setText(m_session.GetHost());
	int portIndex = m_portComboBox->findText(m_session.GetPort());
	if (portIndex != -1) {
		m_portComboBox->setCurrentIndex(portIndex);
	}
	m_proxyLineEdit->setText(m_session.GetProxy());
	m_accessIdLineEdit->setText(m_session.GetAccessId());
	m_secretKeyLineEdit->setText(m_session.GetSecretKey());
}

void
SessionDialog::UpdateSession()
{
	m_session.SetHost(m_hostLineEdit->text().trimmed().toUtf8().constData());
	m_session.SetPort(m_portComboBox->currentText().trimmed().toUtf8().constData());
	m_session.SetProxy(m_proxyLineEdit->text().trimmed().toUtf8().constData());
	m_session.SetAccessId(m_accessIdLineEdit->text().trimmed().toUtf8().constData());
	m_session.SetSecretKey(m_secretKeyLineEdit->text().trimmed().toUtf8().constData());
}

void
SessionDialog::Authenticate()
{
	LOG_INFO("Authenticating session");
	LOG_FILE("                       Authenticating session");

	DisableWidgets();
	QApplication::setOverrideCursor(Qt::BusyCursor);

	if (m_client != NULL) {
		delete m_client;
	}
	m_client = new Client(&m_session);

	if (m_watcher != NULL) {
		delete m_watcher;
	}
	QString anyBucket = "authenticate_ds3_browser_user_by_getting_any_" \
			    "bucket_just_to_verify_the_credentials_" \
			    "the_bucket_does_not_have_to_exist";
	m_watcher = new GetBucketWatcher(QModelIndex(), anyBucket, "");
	connect(m_watcher, SIGNAL(finished()),
		this, SLOT(CheckAuthenticationResponse()));
	QFuture<ds3_get_bucket_response*> future = m_client->GetBucket(anyBucket,
								       "", "",
								       true);
	m_watcher->setFuture(future);
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
		settings.setValue("host", m_session.GetHost());
		settings.setValue("protocol", m_session.GetProtocol());
		settings.setValue("proxy", m_session.GetProxy());
		settings.setValue("port", m_session.GetPort());
		settings.setValue("withCertificateVerification", m_session.GetWithCertificateVerification());
		settings.setValue("accessID", m_session.GetAccessId());
		settings.setValue("secretKey", m_session.GetSecretKey());
	} else {
		settings.remove("");
	}
	settings.endArray();
}

void
SessionDialog::CheckAuthenticationResponse()
{
	bool authenticated = false;
	try {
		ds3_get_bucket_response* response = m_watcher->result();
		if (response) {
			// Will most likely never get here since the bucket
			// we're GET'ing will almost certainly never really
			// exist but it's theoretically possible that it can.
			// Thus, it's automatically.
			authenticated = true;

			ds3_free_bucket_response(response);
		}
	}
	catch (DS3Error& e) {
		QString logPrefix = "Failed to authenticate session - ";
		QString body;
		QString msg;
		switch (e.GetStatusCode()) {
		case 403:
			body = e.GetErrorBody();
			// TODO DS3Error should properly XML decode the body
			if (body.contains("<Code>InvalidSecurity</Code>")) {
				m_accessIdLabel->setStyleSheet("QLabel { color: red; }");
				m_secretKeyLabel->setStyleSheet("QLabel { color: red; }");
			}
			msg = e.ToString();
			m_baseErrorLabel->setText(msg);
			LOG_ERROR(logPrefix + msg);
			LOG_FILE("ERROR:       "+msg);
			m_form->addWidget(m_baseErrorLabel, 0, 0, 1, 3);
			break;
		case 404:
			// Bucket not found is the correct response in this
			// case.  A 403 would have been sent back if the
			// credentials were invalid.
			authenticated = true;
			break;
		default:
			msg = e.ToString();
			m_baseErrorLabel->setText(msg);
			LOG_ERROR(logPrefix + msg);
			LOG_FILE("ERROR:       "+msg);
			m_form->addWidget(m_baseErrorLabel, 0, 0, 1, 3);
		}
	}

	if (authenticated) {
		LOG_INFO("Session authenticated");
		LOG_FILE("                       Session Authenticated");
		SaveSession();
	}
	QApplication::restoreOverrideCursor();
	EnableWidgets();
	if (authenticated) {
		accept();
	}
}
