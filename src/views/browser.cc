#include "views/browser.h"

Browser::Browser(QWidget* parent, Qt::WindowFlags flags)
	: QWidget(parent, flags),
	  m_layout(new QVBoxLayout(this)),
	  m_treeView(new QTreeView)
{
	// Remove the focus rectangle around the tree view on OSX.
	m_treeView->setAttribute(Qt::WA_MacShowFocusRect, 0);

	m_layout->setContentsMargins (0, 0, 0, 0);
	m_layout->addWidget(m_treeView);

	setLayout(m_layout);
}

Browser::~Browser()
{
}
