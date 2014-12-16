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

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

class Dialog : public QDialog
{
	Q_OBJECT

public:
	Dialog(QWidget* parent = 0);
	virtual ~Dialog();

public slots:
	virtual void Accept() = 0;
	void Reject();

protected:
	bool ValidateLineEditNotEmpty(QLabel* label,
				      QLineEdit* lineEdit,
				      QLabel* errorLabel);

	QVBoxLayout* m_layout;
	QGridLayout* m_form;
	QDialogButtonBox* m_buttonBox;
};

#endif
