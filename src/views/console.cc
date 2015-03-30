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

#include <QAction>
#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QTextStream>

#include "lib/logger.h"
#include "views/console.h"

#ifdef NO_DEBUG
#define DEFAULT_LOG_LEVEL INFO
#else
#define DEFAULT_LOG_LEVEL DEBUG
#endif

const QString Console::LEVEL_COLORS[] = { "Blue", "Black", "darkYellow", "Red" };
const unsigned int Console::MAX_LINES = 1000;
Console* Console::s_instance = 0;

Console*
Console::Instance()
{
	if (!s_instance) {
		s_instance = new Console();
	}
	return s_instance;
}

Console::Console(QWidget* parent)
	: QWidget(parent),
	  m_logLevel(DEFAULT_LOG_LEVEL),
	  m_numLines(0)
{
	m_text = new QTextEdit();
	m_text->setReadOnly(true);
	m_text->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_text, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(ShowContextMenu(const QPoint&)));
	m_layout = new QVBoxLayout(this);
	m_layout->addWidget(m_text);
	setLayout(m_layout);

	connect(this, SIGNAL(MessageReadyToLog(int, const QString&)),
		this, SLOT(LogPrivate(int, const QString&)));
}

void
Console::Log(Level level, const QString& msg)
{
	if (level < m_logLevel) {
		return;
	}

	emit MessageReadyToLog(level, msg);
}

void
Console::LogPrivate(int level, const QString& msg)
{
	QString color;
	switch (level) {
	case DEBUG:
		color = "blue";
		break;
	case WARNING:
		color = QColor(175, 175, 0).name();
		break;
	case ERR:
		color = "red";
		break;
	};

	m_lock.lock();
	if (m_numLines >= MAX_LINES) {
		m_text->moveCursor(QTextCursor::Start,
				   QTextCursor::MoveAnchor);
		m_text->moveCursor(QTextCursor::Down,
				   QTextCursor::KeepAnchor);
		m_text->moveCursor(QTextCursor::StartOfLine,
				   QTextCursor::KeepAnchor);
		m_text->textCursor().removeSelectedText();
		m_numLines--;
	}
	m_text->moveCursor(QTextCursor::End,
			   QTextCursor::MoveAnchor);
	QString html = "<font";
	if (!color.isEmpty()) {
		html += " color=\"" + color + "\"";
	}
	html += ">" + msg + "</font><br>";
	m_text->insertHtml(html);
	m_numLines++;
	m_text->ensureCursorVisible();
	m_lock.unlock();
}

void
Console::ShowContextMenu(const QPoint& /*pos*/)
{
	QMenu* menu = m_text->createStandardContextMenu();
	menu->addSeparator();
	QAction save("Save As...", menu);
	menu->addAction(&save);
	QAction* selectedAction = menu->exec(QCursor::pos());

	if (selectedAction == &save) {
		SaveToFile();
	}
}

void
Console::SaveToFile()
{
	QString fileName = QFileDialog::getSaveFileName(this,
							"Save Log",
							QDir::homePath());
	QFile file(fileName);
	if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
		QTextStream stream(&file);
		stream << m_text->toPlainText();
		file.flush();
		file.close();
		LOG_INFO("Saved log to " + fileName);
	} else {
		LOG_ERROR("Unable to save log to " + fileName);
	}
}
