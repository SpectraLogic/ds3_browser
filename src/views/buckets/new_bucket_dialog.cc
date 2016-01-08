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
#include "lib/logger.h"
#include "lib/errors/ds3_error.h"
#include "views/buckets/new_bucket_dialog.h"

NewItemDialog::NewItemDialog(Client* client, QWidget* parent)
	: Dialog(parent),
	  m_client(client)
{
}

void
NewItemDialog::layoutDialog()
{
    m_form->addWidget(m_itemLabel, 1, 0);
    m_form->addWidget(m_itemLineEdit, 1, 1);
    m_form->addWidget(m_itemErrorLabel, 1, 2);
    m_form->addWidget(m_buttonBox, 2, 1, 1, 2);
    m_itemLineEdit->setFocus();
}

void
NewItemDialog::UpdateItemName()
{
    m_itemName = m_itemLineEdit->text().trimmed();
}

void
NewItemDialog::Accept()
{
    if (m_baseErrorLabel == NULL) {
        m_baseErrorLabel = new QLabel;
        m_baseErrorLabel->setStyleSheet("QLabel { color: red; }");
    } else {
        m_baseErrorLabel->setText("");
        m_form->removeWidget(m_baseErrorLabel);
    }
    if (!ValidateLineEditNotEmpty(m_itemLabel,
                      m_itemLineEdit,
                      m_itemErrorLabel)) {
        return;
    }

    UpdateItemName();
    try {
        CreateItem();
    }
    catch (DS3Error& e) {
        QString body;
        QString msg;
        switch (e.GetStatusCode()) {
        case 400:
            msg = "Item \"" + m_itemName + "\" is invalid";
            m_itemLabel->setStyleSheet("QLabel { color: red; }");
            m_itemErrorLabel->setText("is invalid");
            break;
        case 403:
            body = e.GetErrorBody();
            // TODO DS3Error should properly XML decode the body
            if (body.contains("<Code>Forbidden</Code>") &&
                body.contains("spectra-", Qt::CaseInsensitive) &&
                body.contains("reserved", Qt::CaseInsensitive)) {
                m_itemLabel->setStyleSheet("QLabel { color: red; }");
                msg = "Bucket names that start with \"spectra-\" are reserved";
                m_itemErrorLabel->setText(msg);
            }
            break;
        case 409:
            msg = "Item \"" + m_itemName + "\" already exists";
            m_itemLabel->setStyleSheet("QLabel { color: red; }");
            m_itemErrorLabel->setText("already exists");
            break;
        default:
            msg = e.ToString();
            m_baseErrorLabel->setText(msg);
            m_form->addWidget(m_baseErrorLabel, 0, 0, 1, 3);
        }
        LOG_ERROR("ERROR:       CREATE ITEM failed, "+msg);
        return;
    }
    accept();
}


NewBucketDialog::NewBucketDialog(Client* client, QWidget* parent)
    : NewItemDialog(client, parent)
{
    setWindowTitle("New Bucket");
    m_itemLineEdit = new QLineEdit;
    m_itemLineEdit->setToolTip("The name of the Bucket to create");
    m_itemLineEdit->setFixedWidth(150);
    m_itemLabel = new QLabel("Bucket Name");
    m_itemErrorLabel = new QLabel;
    m_itemErrorLabel->setStyleSheet("QLabel { color: red; }");
    layoutDialog();
}

void
NewBucketDialog::CreateItem()
{
    m_client->CreateBucket(m_itemName);
}
