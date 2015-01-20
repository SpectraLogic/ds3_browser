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

#include <QChar>
#include <QFontMetrics>

#include "lib/logger.h"
#include "helpers/number_helper.h"
#include "views/job_view.h"
#include "views/jobs_view.h"

//
// JobView
//

const int JobView::MAX_URLS_WIDTH = 250;
const int JobView::MAX_DEST_WIDTH = 150;
const QString JobView::RIGHT_ARROW = QChar(0x2192);
const QString JobView::s_types[] = { "GET", "PUT" };

JobView::JobView(Job job, QWidget* parent)
	: QWidget(parent),
	  m_jobID(job.GetID())
{
	m_layout = new QGridLayout(this);
	m_layout->setContentsMargins(5, 2, 5, 5);
	m_layout->setSpacing(0);
	m_layout->setAlignment(Qt::AlignTop);
	setLayout(m_layout);

	m_type = new QLabel;
	m_type->setObjectName("type");
	m_host = new QLabel;
	m_start = new QLabel;

	m_urlsAndDestination = new QLabel;

	m_progressBar = new QProgressBar;
	m_progressBar->setMinimum(0);
	m_progressBar->setMaximum(1000);
	m_progressSummary = new QLabel;

	m_cancelButton = new QPushButton;
	m_cancelButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	m_cancelButton->setStyleSheet("border: 0; padding: 0; margin: 0");
	m_cancelButton->setToolTip("Cancel");
	connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(Cancel()));

	Update(job);

	m_layout->addWidget(m_type, 0, 0, 2, 1);
	m_layout->addWidget(m_host, 2, 0);
	m_layout->addWidget(m_start, 3, 0);
	m_layout->addWidget(m_urlsAndDestination, 0, 1);
	m_layout->addWidget(m_progressBar, 1, 1);
	m_layout->addWidget(m_progressSummary, 2, 1);
	m_layout->addWidget(m_cancelButton, 0, 3);

	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setFixedWidth(500);
}

void
JobView::Update(Job job)
{
	m_host->setText(job.GetHost());
	QString urlsAndDest = job.GetURLs();
	QFontMetrics fm(m_urlsAndDestination->font());
	urlsAndDest = fm.elidedText(urlsAndDest, Qt::ElideRight, MAX_URLS_WIDTH);
	urlsAndDest += " " + RIGHT_ARROW + " ";
	QString dest = job.GetDestination();
	dest = fm.elidedText(dest, Qt::ElideRight, MAX_DEST_WIDTH);
	urlsAndDest += dest;
	m_urlsAndDestination->setText(urlsAndDest);
	m_progressBar->setValue(job.GetProgress());
	m_progressSummary->setText(ToProgressSummary(job));
	m_start->setText(job.GetStart().toLocalTime().toString("M/d/yyyy h:mm AP"));
	m_type->setText(ToTypeString(job));
}

const QString
JobView::ToProgressSummary(Job job) const
{
	QString total = NumberHelper::ToHumanSize(job.GetSize());
	uint64_t rawTransferred = job.GetBytesTransferred();
	QString transferred = NumberHelper::ToHumanSize(rawTransferred);

	QDateTime transferStartDT = job.GetTransferStart();
	QString rate;
	if (transferStartDT.isValid()) {
		qint64 transferStart = transferStartDT.toMSecsSinceEpoch();
		qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();
		qint64 elapsed = (now - transferStart) / 1000;
		if (elapsed > 0) {
			rate = NumberHelper::ToHumanRate(rawTransferred / elapsed);
		}
	}

	QString summary = transferred + " of " + total;
	if (!rate.isEmpty()) {
		summary += " - " + rate;
	}
	return summary;
}

void
JobView::Cancel()
{
	LOG_DEBUG("JobView::Cancel");
	emit Canceled();
}

// Necessary since JobView has a styles applied to it via the main QSS file.
// See Qt Style Sheets Reference.
void
JobView::paintEvent(QPaintEvent* /*event*/)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//
// JobsView
//

JobsView::JobsView(QWidget* parent)
	: QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
	m_layout->setAlignment(Qt::AlignTop);
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setSpacing(0);
	setLayout(m_layout);

	// AddDebugJobs();
}

void
JobsView::CancelJob()
{
	JobView* jobView = static_cast<JobView*>(sender());
	QUuid id = jobView->GetJobID();
	LOG_DEBUG("JobsView::CancelJob: " + id.toString());
	emit JobCanceled(id);
}

void
JobsView::UpdateJob(const Job job)
{
	QUuid jobID = job.GetID();

	m_jobsLock.lock();

	if (m_jobViews.contains(jobID)) {
		JobView* jobView = m_jobViews[jobID];
		if (job.IsFinished() || job.WasCanceled()) {
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
		connect(jobView, SIGNAL(Canceled()), this, SLOT(CancelJob()));
		m_jobViews[jobID] = jobView;
		m_layout->addWidget(jobView);
	}

	m_jobsLock.unlock();
}

void
JobsView::AddDebugJobs()
{
	for (int i = 0; i < 10; i++) {
		Job job;
		job.SetID(QUuid::createUuid());
		job.SetType(Job::PUT);
		QString is(QString::number(i));
		job.SetHost("host" + is);
		QList<QUrl> urls;
		urls << QUrl("http://foo.com/bar");
		urls << QUrl("http://foo.com/baz");
		job.SetURLs(urls);
		job.SetDestination("bucket" + is);
		job.SetState(Job::INPROGRESS);
		job.SetStart(QDateTime::currentDateTime());
		job.SetTransferStart(QDateTime::currentDateTime());
		job.SetSize(1000);
		job.SetBytesTransferred(500);
		UpdateJob(job);
	}
}
