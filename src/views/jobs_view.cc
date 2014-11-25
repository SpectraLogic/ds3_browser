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

#include <QProgressBar>
#include <QGridLayout>
#include <QLabel>

#include "lib/logger.h"
#include "helpers/number_helper.h"
#include "views/jobs_view.h"

//
// JobView
//

class JobView : public QWidget
{
public:
	static const QString s_types[];

	JobView(Job job, QWidget* parent = 0);

	void Update(Job job);
	const QString ToProgressSummary(Job) const;
	const QString& ToTypeString(Job) const;

private:
	QLabel* m_host;
	QLabel* m_type;
	QProgressBar* m_progressBar;
	QLabel* m_progressSummary;
	QLabel* m_start;
	QGridLayout* m_layout;
};

const QString JobView::s_types[] = { "GET", "PUT" };

JobView::JobView(Job job, QWidget* parent)
	: QWidget(parent)
{
	m_layout = new QGridLayout(this);
	setLayout(m_layout);

	m_type = new QLabel;
	m_host = new QLabel;
	m_start = new QLabel;

	m_progressBar = new QProgressBar;
	m_progressBar->setMinimum(0);
	m_progressBar->setMaximum(1000);
	m_progressSummary = new QLabel;

	Update(job);

	m_layout->addWidget(m_type, 0, 0, 2, 1);
	m_layout->addWidget(m_host, 1, 0);
	m_layout->addWidget(m_start, 2, 0);
	m_layout->addWidget(m_progressBar, 1, 1);
	m_layout->addWidget(m_progressSummary, 2, 1);
}

void
JobView::Update(Job job)
{
	m_host->setText(job.GetHost());
	m_progressBar->setValue(job.GetProgress());
	m_progressSummary->setText(ToProgressSummary(job));
	m_start->setText(job.GetStart().toLocalTime().toString("M/d/yyyy h:mm AP"));
	m_type->setText(ToTypeString(job));
}

inline const QString
JobView::ToProgressSummary(Job job) const
{
	QString total = NumberHelper::ToHumanSize(job.GetSize());
	QString transferred = NumberHelper::ToHumanSize(job.GetBytesTransferred());
	QString summary = transferred + " of " + total;
	return summary;
}

inline const QString&
JobView::ToTypeString(Job job) const
{
	return s_types[job.GetType()];
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
