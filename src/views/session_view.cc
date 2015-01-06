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
#include "models/session.h"
#include "views/ds3_browser.h"
#include "views/host_browser.h"
#include "views/jobs_view.h"
#include "views/session_view.h"

SessionView::SessionView(Session* session, JobsView* jobsView, QWidget* parent)
	: QWidget(parent),
	  m_session(session)
{
	m_client = new Client(session);
	connect(jobsView, SIGNAL(JobCanceled(QUuid)),
		m_client, SLOT(CancelBulkJob(QUuid)));

	m_hostBrowser = new HostBrowser(m_client);
	m_ds3Browser = new DS3Browser(m_client, jobsView);

	m_splitter = new QSplitter;
	m_splitter->addWidget(m_hostBrowser);
	m_splitter->addWidget(m_ds3Browser);
	m_splitter->setCollapsible(0, false);
	m_splitter->setCollapsible(1, false);

	m_topLayout = new QVBoxLayout(this);
	m_topLayout->setContentsMargins(5, 5, 5, 5);
	m_topLayout->addWidget(m_splitter);

	setLayout(m_topLayout);
}

SessionView::~SessionView()
{
	delete m_client;
	delete m_session;
}

int
SessionView::GetNumActiveJobs() const
{
	return m_client->GetNumActiveJobs();
}

void
SessionView::CancelActiveJobs()
{
	m_client->CancelActiveJobs();
}
