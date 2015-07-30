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
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QTextStream>
#include <QSettings>

#include "lib/logger.h"
#include "views/console.h"

#ifdef NO_DEBUG
#define DEFAULT_LOG_LEVEL INFO
#else
#define DEFAULT_LOG_LEVEL DEBUG
#endif

static const QString LOG_TIMESTAMP_FORMAT = "MMMM d h:mm:ss";
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
	bool fileLogging = true;
	QString color;
	switch (level) {
	case DEBUG:
		fileLogging = false;
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
	if(!msg.startsWith("     ")) {
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
	}

	if(fileLogging)
		LogToFile(msg);

	m_lock.unlock();
}

void
Console::LogToFile(QString message)
{
	bool overwrite = false;
	QSettings settings;
	bool loggingEnabled = settings.value("mainWindow/loggingEnabled", true).toBool();
	if(!loggingEnabled)
		return;

	QString currentTimeStamp = QDateTime::currentDateTime().toString(LOG_TIMESTAMP_FORMAT);
	message = currentTimeStamp+": "+message;

	QString filename = settings.value("mainWindow/logFileName").toString();
	QFile* file = new QFile(filename);
	double logSize = settings.value("mainWindow/logFileSize", 52428800).toDouble();
	
	// Checking if log size is over 50MBs
	if(file->size() >= logSize) {
		// Close and delete log file
		delete file;
		ArchiveLog(filename);
		// After archiving is done, get a new file handle
		file = new QFile(filename);
		overwrite = true;
	}
	if(overwrite) {
		if(file->open(QIODevice::WriteOnly | QIODevice::Text)) {
			QTextStream stream(file);
			stream << message << endl;
		}
	} else if(file->open(QIODevice::Append | QIODevice::Text)) {
		QTextStream stream(file);
		stream << message << endl;
	}
	delete file;
}

void
Console::ArchiveLog(QString filename)
{
	// Get everything after and including the last "." and save as the filetype
	QString filetype = filename.mid(filename.lastIndexOf("."), -1);
	QString filesanstype = filename.mid(0, filename.lastIndexOf("."));
	// Function that increments archives
	IncrementLog(filesanstype, filetype, qint64(1));
}

void
Console::IncrementLog(QString filename, QString filetype, qint64 number)
{
	QSettings settings;
	int logNumberLimit = settings.value("mainWindow/logNumberLimit", 10).toInt();

	// Flag for whether or not the current file is zipped or not
	bool compressed = true;
	// File being read and moved into the zip created soon
	QFile* oldFile;
	// This means that the file being moved into the zip that will soon be
	//   created is just a .log file, not a zip
	if(number == 1) {
		compressed = false;
		oldFile = new QFile(filename+filetype);
	}
	// Every other file after the first one is a zip file
	else {
		oldFile = new QFile(filename+filetype+"."+QString::number(number-1)+".zip");
	}
	// Output zip file
	QFile* newFile = new QFile(filename+filetype+"."+QString::number(number)+".zip");
	// Recursively call this function until the highest unused file is found
	if(newFile->exists() && number < logNumberLimit) {
		IncrementLog(filename, filetype, number+1);
	}
	// No longer need this handle, so delete it
	delete newFile;
	// This case compresses the normal log file into the first archive
	if(!compressed && oldFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
		QByteArray fileData = oldFile->readAll();
		delete oldFile;
		// Zip handle for current archive
		QuaZip zip(filename+filetype+"."+QString::number(number)+".zip");
		zip.setFileNameCodec("IBM866");
		if(!zip.open(QuaZip::mdCreate)) {
			LogPrivate(DEBUG, "ERROR: could not create archive '"+filename+filetype+"."+QString::number(number)+".zip'");
			return;
		}
		QFileInfo file;
		file = QFileInfo(filename+filetype+"."+QString::number(number));

		QuaZipFile outFile(&zip);
		QString fileNameWithRelativePath = file.filePath().remove(0, filename.lastIndexOf("/") + 1);
		QuaZipNewInfo newInfo(fileNameWithRelativePath, file.filePath());
		// Make sure that the compressed file can be read after being extracted
		newInfo.setPermissions(QFileDevice::ReadOwner | QFileDevice::ReadUser);
		if (!outFile.open(QIODevice::WriteOnly, newInfo)) {
			LogPrivate(DEBUG, "ERROR: could not create the zipped file '"+filename+filetype+"."+QString::number(number)+"'");
			zip.close();
		        return;
		}
		fileData += "logfile turned over due to size>50MB\n";
		outFile.write(fileData);
		// Zip file and catch any errors
		if(outFile.getZipError() != UNZ_OK) {
			LogPrivate(DEBUG, "ERROR: could not add log to the archive '"+filename+filetype+"."+QString::number(number)+".zip'");
			outFile.close();
			zip.close();
			return;
		}
		outFile.close();
		zip.close();
	// This case moves each archive to the next number
	} else {
		delete oldFile;
		// Zip handle for previous archive
		QuaZip oldZip(filename+filetype+"."+QString::number(number-1)+".zip");
		if(!oldZip.open(QuaZip::mdUnzip)) {
			LogPrivate(DEBUG, "ERROR: could not unzip archive '"+filename+filetype+"."+QString::number(number-1)+".zip'");
			return;
		}
		// Zip handle for current archive
		QuaZip newZip(filename+filetype+"."+QString::number(number)+".zip");
		newZip.setFileNameCodec("IBM866");
		if(!newZip.open(QuaZip::mdCreate)) {
			LogPrivate(DEBUG, "ERROR: could not create archive '"+filename+filetype+"."+QString::number(number)+".zip'");
			oldZip.close();
			return;
		}

		QuaZipFileInfo info;
		QuaZipFile inFile(&oldZip);

		QFileInfo file;
		file = QFileInfo(filename+filetype+"."+QString::number(number));

		QuaZipFile outFile(&newZip);
		QString fileNameWithRelativePath = file.filePath().remove(0, filename.lastIndexOf("/") + 1);
		QuaZipNewInfo newInfo(fileNameWithRelativePath, file.filePath());
		// Make sure that the compressed file can be read after being extracted
		newInfo.setPermissions(QFileDevice::ReadOwner | QFileDevice::ReadUser);
		if (!outFile.open(QIODevice::WriteOnly, newInfo)) {
			LogPrivate(DEBUG, "ERROR: could not create the zipped file '"+filename+filetype+"."+QString::number(number)+"'");
		        oldZip.close();
			newZip.close();
		        return;
		}

		// Get the handle to the compressed log file which should be the first
		//   and only file in the archive
		oldZip.goToFirstFile();
		if (!oldZip.getCurrentFileInfo(&info)) {
			LogPrivate(DEBUG, "ERROR: could not get file information from archive '"+filename+filetype+"."+QString::number(number-1)+".zip'");
		        outFile.close();
			oldZip.close();
			newZip.close();
		        return;
		}

		if(!inFile.open(QIODevice::ReadOnly)) {
			LogPrivate(DEBUG, "ERROR: could not open archived file '"+filename+filetype+"."+QString::number(number-1)+"'");
			outFile.close();
			oldZip.close();
			newZip.close();
			return;
		}
		outFile.write(inFile.readAll());

		// Zip file and catch any errors
		if(outFile.getZipError() != UNZ_OK) {
			LogPrivate(DEBUG, "ERROR: could not add log to the archive '"+filename+filetype+"."+QString::number(number)+".zip'");
			outFile.close();
			inFile.close();
			oldZip.close();
			newZip.close();
			return;
		}
		outFile.close();
		inFile.close();
		oldZip.close();
		newZip.close();
	}
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
