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

#ifndef CONSOLE_H
#define CONSOLE_H

#include <QMutex>
#include <QString>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

class Console : public QWidget
{
	Q_OBJECT

public:
	enum Level { DEBUG, INFO, WARNING, ERROR };
	static const QString LEVEL_COLORS[];
	static const unsigned int MAX_LINES;

	static Console* Instance();

	void Log(Level level, const QString& msg);

signals:
	void MessageReadyToLog(int level, const QString& msg);

private:
	Console(QWidget* parent = 0);

	static Console* s_instance;

	QMutex m_lock;
	Level m_logLevel;
	unsigned int m_numLines;
	QTextEdit* m_text;
	QVBoxLayout* m_layout;

private slots:
	void LogPrivate(int level, const QString& msg);
};

#endif
