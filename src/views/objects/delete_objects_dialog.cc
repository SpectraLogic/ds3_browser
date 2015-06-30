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

#include <ds3.h>

#include "lib/client.h"
#include "lib/logger.h"
#include "views/objects/delete_objects_dialog.h"

DeleteObjectsDialog::DeleteObjectsDialog(Client* client,
					 const QString& bucketName,
					 const QStringList& objectNames,
					 QWidget* parent)
	: DS3DeleteDialog(client, bucketName, true, parent),
	  m_objectNames(objectNames)
{
	QString title = "Delete Object";
	if (m_objectNames.count() > 1) {
		title += "s";
	}
	setWindowTitle(title);

	QString warning = "This action will delete the ";
	if (m_objectNames.count() == 1) {
		warning += "\"" + m_objectNames[0] + "\" object.";
	} else {
		warning += "following objects: " + m_objectNames.join(",");
		warning += ".";
	}
	warning += " Are you sure you wish to continue?";
	m_warning->setText(warning);
}

bool
DeleteObjectsDialog::Delete()
{
	try {
		// TODO Handle multiple object deletion
		if (m_objectNames.count() == 1) {
			m_client->DeleteObject(m_bucketName, m_objectNames[0]);
		}
	}
	catch (DS3Error& e) {
		QString body;
		QString msg;
		switch (e.GetStatusCode()) {
		case 403:
			body = e.GetErrorBody();
			if (body.contains("spectra-", Qt::CaseInsensitive)) {
				msg = "Buckets that start with \"spectra-\" " \
				      "are reserved and objects within then " \
				      "cannot be deleted";
			}
			break;
		}
		if (msg.isEmpty()) {
			msg = e.ToString();
		}
		m_baseErrorLabel->setText(msg);
		m_form->addWidget(m_baseErrorLabel, 0, 0, 1, 3);
		LOG_ERROR("Error deleting objects - " + msg);
		LOG_FILE("ERROR:       DELETE OBJECTS failed, "+msg);
		return false;
	}

	return true;
}
