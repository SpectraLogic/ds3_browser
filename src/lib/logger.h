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

#ifndef LOGGER_H
#define LOGGER_H

#include <QMutex>
#include <QString>
#include <QTextEdit>

#define LOG_DEBUG(msg)   LOG(Logger::DEBUG,   msg)
#define LOG_INFO(msg)    LOG(Logger::INFO,    msg)
#define LOG_WARNING(msg) LOG(Logger::WARNING, msg)
#define LOG_ERROR(msg)   LOG(Logger::ERROR,   msg)
#define LOG(level, msg)  Logger::Instance()->Log(level, msg)

class Logger
{
public:
	enum Level { DEBUG, INFO, WARNING, ERROR };
	static const QString LEVEL_COLORS[];

	static Logger* Instance();

	void Log(Level level, const QString& msg);

	void SetStream(QTextEdit* m_stream);

private:
	Logger();

	static Logger* s_instance;

	Level m_logLevel;
	QTextEdit* m_stream;
	QMutex* m_streamLock;
};

#endif
