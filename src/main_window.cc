#include <QMenuBar>
#include <QMessageBox>

#include "main_window.h"
#include "models/session.h"
#include "views/session_dialog.h"
#include "views/session_view.h"

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags),
	  m_isFinished(false),
	  m_sessionTabs(new QTabWidget(this))
{
	setWindowTitle("Spectra Logic DS3 Explorer");

	Session* session = CreateSession();
	if (session == 0)
	{
		// User closed/cancelled the New Session dialog which should
		// result in the application closing.
		m_isFinished = true;
		return;
	}

	CreateMenus();

	m_sessionView = new SessionView(session, this);
	m_sessionTabs->addTab(m_sessionView,
			      QString::fromStdString(session->GetHost()));
	setCentralWidget(m_sessionTabs);
}

Session*
MainWindow::CreateSession()
{
	SessionDialog sessionDialog;
	if (sessionDialog.exec() == QDialog::Rejected) {
		return 0;
	}
	Session* session = new Session(*sessionDialog.GetSession());
	return session;
}

void
MainWindow::CreateMenus()
{
	m_aboutAction = new QAction(tr("&About"), this);
	connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(About()));

	m_helpMenu = new QMenu(tr("&Help"), this);
	m_helpMenu->addAction(m_aboutAction);

	menuBar()->addMenu(m_helpMenu);
}

void
MainWindow::About()
{
	QString text = tr("<b>DS3 Explorer</b><br/>Version %1")
				.arg(APP_VERSION);
	QMessageBox::about(this, tr("About DS3 Explorer"), text);
}
