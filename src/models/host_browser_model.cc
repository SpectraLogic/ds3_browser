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
#include "lib/mime_data.h"
#include "models/host_browser_model.h"

HostBrowserModel::HostBrowserModel(Client* client, QObject* parent)
	: QFileSystemModel(parent),
	  m_client(client)
{
}

QStringList
HostBrowserModel::mimeTypes() const
{
	QStringList types = QFileSystemModel::mimeTypes();
	types << MimeData::DS3_MIME_TYPE;
	return types;
}

bool
HostBrowserModel::dropMimeData(const QMimeData* data,
			       Qt::DropAction action,
			       int row, int column,
			       const QModelIndex& parentIndex)
{
	const MimeData* mimeData = qobject_cast<const MimeData*>(data);
	if (!mimeData) {
		// Could happen when you drag a file onto itself
		return false;
	}
	if (!mimeData->HasDS3URLs()) {
		return QFileSystemModel::dropMimeData(data, action, row, column, parentIndex);
	}

	QList<QUrl> urls = mimeData->GetDS3URLs();
	QString destination = filePath(parentIndex);
	m_client->BulkGet(urls, destination);

	return true;
}
