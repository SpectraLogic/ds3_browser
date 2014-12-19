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

#ifndef JOBS_VIEW_H
#define JOBS_VIEW_H

#include <QHash>
#include <QList>
#include <QMutex>
#include <QString>
#include <QUuid>
#include <QVBoxLayout>
#include <QWidget>

#include "models/job.h"

class JobView;

class JobsView : public QWidget
{
	Q_OBJECT

public:
	JobsView(QWidget* parent = 0);

public slots:
	void CancelJob();
	void UpdateJob(const Job job);

signals:
	void JobCanceled(QUuid id);

private:
	// Add some debug JobView widgets.  Useful when working on the
	// look/feel of JobView.
	void AddDebugJobs();

	QHash<QUuid, JobView*> m_jobViews;
	mutable QMutex m_jobsLock;
	QVBoxLayout* m_layout;
};

#endif
