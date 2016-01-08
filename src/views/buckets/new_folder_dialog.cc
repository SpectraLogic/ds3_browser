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
#include "views/buckets/new_folder_dialog.h"

NewFolderDialog::NewFolderDialog(Client* client, QString bucket, QString folder, QWidget* parent)
    : NewItemDialog(client, parent)
{
    setWindowTitle("New Folder in " + bucket + "/" + folder);
    m_bucketName = bucket;
    m_folderName = folder;

    m_itemLineEdit = new QLineEdit;
    m_itemLineEdit->setToolTip("The name of the Folder to create");
    m_itemLineEdit->setFixedWidth(150);
    m_itemLabel = new QLabel("Folder Name");
    m_itemErrorLabel = new QLabel;
    m_itemErrorLabel->setStyleSheet("QLabel { color: red; }");
    layoutDialog();
}

void
NewFolderDialog::CreateItem()
{
    if (m_folderName.length() > 0) {
        m_itemName = m_folderName + "/" + m_itemName;
    }
    m_client->CreateFolder(m_bucketName, m_itemName);
}
