#include "views/host_browser.h"

HostBrowser::HostBrowser(QWidget* parent, Qt::WindowFlags flags)
	: Browser(parent, flags),
	  m_model(new QFileSystemModel(this))
{
	AddCustomToolBarActions();

	QString rootPath = m_model->myComputer().toString();
	m_path->setText(rootPath);
	m_model->setRootPath(rootPath);
	m_model->setFilter(QDir::AllDirs |
			   QDir::AllEntries |
			   QDir::NoDotAndDotDot |
			   QDir::Hidden);
	m_treeView->setModel(m_model);

	m_treeView->setExpandsOnDoubleClick(false);
	m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	connect(m_treeView, SIGNAL(doubleClicked(const QModelIndex&)),
		this, SLOT(OnModelItemDoubleClick(const QModelIndex&)));
}

void
HostBrowser::AddCustomToolBarActions()
{
	m_homeAction = new QAction(style()->standardIcon(QStyle::SP_DirHomeIcon),
				   "Home", this);
	connect(m_homeAction, SIGNAL(triggered()), this, SLOT(GoToHome()));
	m_toolBar->addAction(m_homeAction);
}

void
HostBrowser::GoToHome()
{
	QString path = QDir::homePath();
	m_treeView->setRootIndex(m_model->index(path));
	m_path->setText(path);
}

void
HostBrowser::GoToRoot()
{
	QVariant myComputer = m_model->myComputer();
	m_treeView->setRootIndex(myComputer.toModelIndex());
	m_path->setText(myComputer.toString());
}

void
HostBrowser::OnModelItemDoubleClick(const QModelIndex& index)
{
	QString path = m_model->filePath(index);
	QDir dir = QDir(path);
	if (m_model->isDir(index) && dir.isReadable())
	{
		m_treeView->setRootIndex(index);
		m_path->setText(path);
	}
}
