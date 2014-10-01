#include "views/host_browser.h"

HostBrowser::HostBrowser(QWidget* parent, Qt::WindowFlags flags)
	: Browser(parent, flags),
	  m_model(new QFileSystemModel(this))
{
	QString rootPath = m_model->myComputer().toString();
	m_model->setRootPath(rootPath);
	m_model->setFilter(QDir::AllDirs |
			   QDir::AllEntries |
			   QDir::NoDotAndDotDot |
			   QDir::Hidden);
	m_treeView->setModel(m_model);
}
