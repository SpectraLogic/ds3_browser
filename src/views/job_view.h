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

#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QProgressBar>
#include <QString>
#include <QStyleOption>
#include <QWidget>

#include "models/job.h"

class JobView : public QWidget
{
	Q_OBJECT

public:
	static const QString s_types[];

	JobView(Job job, QWidget* parent = 0);

	void Update(Job job);
	const QString ToProgressSummary(Job) const;
	const QString& ToTypeString(Job) const;

protected:
	void paintEvent(QPaintEvent* event);

private:
	QLabel* m_host;
	QLabel* m_type;
	QProgressBar* m_progressBar;
	QLabel* m_progressSummary;
	QLabel* m_start;
	QGridLayout* m_layout;
};

inline const QString&
JobView::ToTypeString(Job job) const
{
	return s_types[job.GetType()];
}
