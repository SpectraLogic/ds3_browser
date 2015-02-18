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
#include "views/ds3_delete_dialog.h"

const QString DS3DeleteDialog::CONFIRM_WORD = "DELETE";

DS3DeleteDialog::DS3DeleteDialog(Client* client,
				 const QString& bucketName,
				 bool confirmationRequired,
				 QWidget* parent)
	: Dialog(parent),
	  m_client(client),
	  m_bucketName(bucketName),
	  m_confirmationRequired(confirmationRequired)
{
	m_warning = new QLabel();
	m_warning->setWordWrap(true);
	m_form->addWidget(m_warning, 1, 0);

	int buttonBoxRow = 2;
	if (m_confirmationRequired) {
		m_confirmLabel = new QLabel(tr("Enter %1 to confirm").arg(CONFIRM_WORD));
		m_form->addWidget(m_confirmLabel, 2, 0, Qt::AlignHCenter);

		m_confirmInput = new QLineEdit;
		m_confirmInput->setFixedWidth(150);
		m_form->addWidget(m_confirmInput, 3, 0, Qt::AlignHCenter);

		m_confirmInput->setFocus();

		buttonBoxRow += 2;
	}

	m_form->addWidget(m_buttonBox, buttonBoxRow, 0);
}

void
DS3DeleteDialog::Accept()
{
	if (m_baseErrorLabel == NULL) {
		m_baseErrorLabel = new QLabel;
		m_baseErrorLabel->setStyleSheet("QLabel { color: red; }");
	} else {
		m_baseErrorLabel->setText("");
		m_form->removeWidget(m_baseErrorLabel);
	}
	if (m_confirmationRequired && m_confirmInput->text() != CONFIRM_WORD) {
		m_confirmLabel->setStyleSheet("QLabel { color: red; }");
		m_confirmInput->setFocus();
	}

	bool deleteSuccessful = Delete();
	if (deleteSuccessful) {
		accept();
	}
}
