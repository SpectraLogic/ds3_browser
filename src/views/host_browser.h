#ifndef HOST_BROWSER_H
#define HOST_BROWSER_H

#include <QFileSystemModel>
#include <QList>

#include "views/browser.h"

// HostBrowser, a Browser class used for the local/host system
class HostBrowser : public Browser
{
	Q_OBJECT

public:
	HostBrowser(QWidget* parent = 0, Qt::WindowFlags flags = 0);

protected:
	void AddCustomToolBarActions();
	void UpdatePathLabel(const QString& path);
	void GoToParent();
	void GoToRoot();
	void OnContextMenuRequested(const QPoint& pos);
	void OnModelItemDoubleClick(const QModelIndex& index);

private:
	QAction* m_homeAction;
	QAction* m_uploadAction;
	QFileSystemModel* m_model;

	QList<QString> GetSelectedFiles();

private slots:
	void GoToHome();
};

#endif
