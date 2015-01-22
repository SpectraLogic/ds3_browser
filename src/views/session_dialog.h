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

#ifndef SESSION_DIALOG_H
#define SESSION_DIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>

#include "models/session.h"
#include "views/dialog.h"

class Client;
class GetBucketWatcher;

// SessionDialog, a dialog to gather host/user information when creating
// a new host<->DS3 System session.
class SessionDialog : public Dialog
{
	Q_OBJECT

public:
	SessionDialog(QWidget* parent = 0);
	~SessionDialog();

	const Session& GetSession() const;

public slots:
	void Accept();

private:
	void LoadSession();
	void UpdateSession();
	void Authenticate();
	void SaveSession();

	QWidget* m_headerWidget;
	QGridLayout* m_header;
	QLabel* m_bpLogoLabel;
	QLabel* m_spectraLogoLabel;

	QLabel* m_hostLabel;
	QLineEdit* m_hostLineEdit;
	QLabel* m_hostErrorLabel;
	QLabel* m_portLabel;
	QComboBox* m_portComboBox;
	QLabel* m_proxyLabel;
	QLineEdit* m_proxyLineEdit;
	QLabel* m_accessIdLabel;
	QLineEdit* m_accessIdLineEdit;
	QLabel* m_accessIdErrorLabel;
	QLabel* m_secretKeyLabel;
	QLineEdit* m_secretKeyLineEdit;
	QLabel* m_secretKeyErrorLabel;

	QCheckBox* m_saveSessionCheckBox;

	Client* m_client;
	Session m_session;
	GetBucketWatcher* m_watcher;

private slots:
	void CheckAuthenticationResponse();
};

inline const Session&
SessionDialog::GetSession() const
{
	return m_session;
}

#endif
