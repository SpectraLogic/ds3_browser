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

#ifndef SESSION_VIEW_H
#define SESSION_VIEW_H

#include <QVBoxLayout>
#include <QSplitter>
#include <QWidget>
#include <QMimeData>

class DS3Browser;
class HostBrowser;
class JobsView;
class Session;
class Client;

// SessionView, the overall view that is used for each session.  It represents
// the host and DS3 system in a split pane with the host on the left and
// DS3 system on the right.
class SessionView : public QWidget
{
	Q_OBJECT

public:
	SessionView(Session* session, JobsView* jobsView, QWidget* parent = 0);
	virtual ~SessionView();

	int GetNumActiveJobs() const;
	void CancelActiveJobs();

private:
	DS3Browser* m_ds3Browser;
	HostBrowser* m_hostBrowser;
	Session* m_session;
	Client* m_client;

	QVBoxLayout* m_topLayout;
	QSplitter* m_splitter;

private slots:
	void HostToDS3();
	void DS3ToHost();
	void SendToDS3(QMimeData* data);
	void SendToHost(QMimeData* data);
};

#endif
