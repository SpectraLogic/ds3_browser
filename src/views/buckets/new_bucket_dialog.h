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

#ifndef NEW_BUCKET_DIALOG_H
#define NEW_BUCKET_DIALOG_H

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

class Client;

class NewBucketDialog : public QDialog
{
	Q_OBJECT

public:
	NewBucketDialog(Client* client, QWidget* parent = 0);

	const QString& GetBucket() const;

public slots:
	void Accept();
	void Reject();

private:
	bool ValidateLineEditNotEmpty(QLabel* label, QLineEdit* lineEdit);
	void UpdateBucket();
	void CreateBucket();

	QVBoxLayout* m_layout;
	QGridLayout* m_form;
	QLabel* m_bucketLabel;
	QLabel* m_bucketErrorLabel;
	QLabel* m_baseErrorLabel;
	QLineEdit* m_bucketLineEdit;
	QDialogButtonBox* m_buttonBox;

	Client* m_client;
	QString m_bucket;
};

inline const QString&
NewBucketDialog::GetBucket() const
{
	return m_bucket;
}

#endif
