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

#ifndef SESSION_DIALOG_H
#define SESSION_DIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

class Session;

// SessionDialog, a dialog to gather host/user information when creating
// a new host<->DS3 System session.
class SessionDialog : public QDialog
{
	Q_OBJECT

public:
	SessionDialog(QWidget* parent = 0);
	~SessionDialog();

	Session* GetSession() const;

public slots:
	void Accept();
	void Reject();

private:
	bool ValidateLineEditNotEmpty(QLabel*, QLineEdit*);
	void LoadSession();
	void UpdateSession();
	void SaveSession();

	QVBoxLayout* m_layout;

	QFormLayout* m_form;

	QLabel* m_hostLabel;
	QLineEdit* m_hostLineEdit;
	QComboBox* m_portComboBox;
	QLabel* m_proxyLabel;
	QLineEdit* m_proxyLineEdit;
	QLabel* m_accessIdLabel;
	QLineEdit* m_accessIdLineEdit;
	QLabel* m_secretKeyLabel;
	QLineEdit* m_secretKeyLineEdit;

	QCheckBox* m_saveSessionCheckBox;
	QDialogButtonBox* m_buttonBox;

	Session* m_session;
};

inline Session*
SessionDialog::GetSession() const
{
	return m_session;
}

#endif
