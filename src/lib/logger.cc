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

#include <iostream>

#include "lib/logger.h"

#ifdef NO_DEBUG
#define DEFAULT_LOG_LEVEL INFO
#else
#define DEFAULT_LOG_LEVEL DEBUG
#endif

const QString Logger::LEVEL_COLORS[] = { "Blue", "Black", "darkYellow", "Red" };
Logger* Logger::s_instance = 0;

Logger*
Logger::Instance()
{
	if (!s_instance) {
		s_instance = new Logger();
	}
	return s_instance;
}

Logger::Logger()
	: m_logLevel(DEFAULT_LOG_LEVEL),
	  m_stream(0),
	  m_streamLock(new QMutex)
{
}

void
Logger::Log(Level level, const QString& msg)
{
	if (level < m_logLevel) {
		return;
	}

	QString fullMsg(msg);
	m_streamLock->lock();
	if (!m_stream) {
		std::cout << fullMsg.toUtf8().constData() << std::endl;
	}

	QColor oldColor = m_stream->textColor();
	QColor color;
	switch (level) {
	case DEBUG:
		color = QColor("Blue");
		break;
	case INFO:
		color = QColor("Black");
		break;
	case WARNING:
		color = QColor(175, 175, 0);
		break;
	case ERROR:
		color = QColor("Red");
		break;
	};
	m_stream->setTextColor(color);
	m_stream->append(fullMsg);
	m_stream->setTextColor(oldColor);

	m_streamLock->unlock();
}

void
Logger::SetStream(QTextEdit* stream)
{
	m_streamLock->lock();
	m_stream = stream;
	m_streamLock->unlock();
}
