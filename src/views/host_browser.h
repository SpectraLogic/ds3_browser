#ifndef HOST_BROWSER_H
#define HOST_BROWSER_H

#include <QFileSystemModel>
#include "views/browser.h"

// HostBrowser, a Browser class used for the local/host system
class HostBrowser : public Browser
{
public:
	HostBrowser(QWidget* parent = 0, Qt::WindowFlags flags = 0);

private:
	QFileSystemModel* m_model;
};

#endif
