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

#ifndef DS3_DELETE_DIALOG_H
#define DS3_DELETE_DIALOG_H

#include <QLabel>
#include <QLineEdit>
#include <QString>

#include "views/dialog.h"

class Client;

class DS3DeleteDialog : public Dialog
{
	Q_OBJECT

public:
	static const QString CONFIRM_WORD;

	DS3DeleteDialog(Client* client,
			const QString& bucketName,
			bool confirmationRequired,
			QWidget* parent = 0);

public slots:
	void Accept();

protected:
	Client* m_client;
	QString m_bucketName;

	QLabel* m_warning;

	virtual bool Delete() = 0;

private:
	bool m_confirmationRequired;

	QLabel* m_confirmLabel;
	QLineEdit* m_confirmInput;
};

#endif
