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

#ifndef NEW_BUCKET_DIALOG_H
#define NEW_BUCKET_DIALOG_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>

#include "views/dialog.h"

class Client;

class NewItemDialog : public Dialog
{
	Q_OBJECT

public:
	NewItemDialog(Client* client, QWidget* parent = 0);

public slots:
	void Accept();

protected:
	void UpdateItemName();
	virtual void CreateItem() = 0;

	void layoutDialog();
	const QString& GetItemName() const;

	QLabel* m_itemLabel;
	QLabel* m_itemErrorLabel;
	QLineEdit* m_itemLineEdit;

	Client* m_client;
	QString m_itemName;
};


class NewBucketDialog : public NewItemDialog
{
	Q_OBJECT

public:
	NewBucketDialog(Client* client, QWidget* parent = 0);

protected:
	void UpdateItem();
	void CreateItem();
};

#endif
