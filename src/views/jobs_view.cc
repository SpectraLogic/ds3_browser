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

#include <QApplication>
#include <QItemDelegate>
#include <QProgressBar>
#include <QStyle>
#include <QStyleOptionProgressBar>
#include <QListWidgetItem>

#include "lib/logger.h"
#include "views/jobs_view.h"

//
// JobView
//

class JobView : public QWidget
{
public:
	JobView(Job job, QWidget* parent = 0);

	void Update(Job job);

private:
	QVBoxLayout* m_layout;
	QProgressBar* m_bar;
};

JobView::JobView(Job job, QWidget* parent)
	: QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
	setLayout(m_layout);

	m_bar = new QProgressBar();
	m_bar->setMinimum(0);
	m_bar->setMaximum(100);

	Update(job);

	m_layout->addWidget(m_bar);
}

void
JobView::Update(Job job)
{
	m_bar->setValue(job.GetProgress());
}

//
// JobsView
//

JobsView::JobsView(QWidget* parent)
	: QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
	setLayout(m_layout);
}

void
JobsView::UpdateJob(const Job job)
{
	QUuid jobID = job.GetID();

	m_jobsLock.lock();

	if (m_jobViews.contains(jobID)) {
		JobView* jobView = m_jobViews[jobID];
		if (job.IsFinished()) {
			// Remove the job if it's finished.  We could also
			// leave it, showing it in a finished state and give
			// the user the option of manually removing it.
			m_layout->removeWidget(jobView);
			m_jobViews.remove(jobID);
			delete jobView;
			m_jobsLock.unlock();
			return;
		}
		jobView->Update(job);
	} else {
		JobView* jobView = new JobView(job, this);
		m_jobViews[jobID] = jobView;
		m_layout->addWidget(jobView);
	}

	m_jobsLock.unlock();
}
