#include "views/browser.h"

Browser::Browser(QWidget* parent, Qt::WindowFlags flags)
	: QWidget(parent, flags),
	  m_layout(new QVBoxLayout(this)),
	  m_path(new QLabel("/")),
	  m_toolBar(new QToolBar),
	  m_treeView(new QTreeView)
{
	AddToolBarActions();

	m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_treeView, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(OnContextMenuRequested(const QPoint&)));

	// Remove the focus rectangle around the tree view on OSX.
	m_treeView->setAttribute(Qt::WA_MacShowFocusRect, 0);

	m_layout->setContentsMargins (0, 0, 0, 0);
	m_layout->addWidget(m_toolBar);
	m_layout->addWidget(m_path);
	m_layout->addWidget(m_treeView);

	setLayout(m_layout);
}

Browser::~Browser()
{
}

void
Browser::AddToolBarActions()
{
	m_parentAction = new QAction(style()->standardIcon(QStyle::SP_FileDialogToParent),
				     "Parent directory", this);
	connect(m_parentAction, SIGNAL(triggered()), this, SLOT(GoToParent()));
	m_toolBar->addAction(m_parentAction);

	m_rootAction = new QAction(style()->standardIcon(QStyle::SP_ComputerIcon),
				   "Root directory", this);
	connect(m_rootAction, SIGNAL(triggered()), this, SLOT(GoToRoot()));
	m_toolBar->addAction(m_rootAction);
}

void
Browser::UpdatePathLabel(const QString& path)
{
	m_path->setText(path);
}
