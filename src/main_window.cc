#include <QMenuBar>
#include <QMessageBox>

#include "main_window.h"
#include "models/session.h"

MainWindow::MainWindow(Session* session)
{
	setWindowTitle("Spectra Logic DS3 Explorer");

	CreateMenus();

	m_sessionView = new SessionView(session, this);
	setCentralWidget(m_sessionView);
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
