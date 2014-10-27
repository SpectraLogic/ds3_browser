#ifndef DS3_BROWSER_H
#define DS3_BROWSER_H

#include "views/browser.h"

class Bucket;
class Client;
class Session;

// DS3Browser, a Browser class used for browsing a DS3 system (e.g a
// BlackPearl)
class DS3Browser : public Browser
{
	Q_OBJECT

public:
	DS3Browser(Session* session,
		   QWidget* parent = 0,
		   Qt::WindowFlags flags = 0);
	~DS3Browser();

protected:
	void GoToParent();
	void GoToRoot();
	void OnContextMenuRequested(const QPoint& pos);
	void OnModelItemDoubleClick(const QModelIndex& index);

private:
	Bucket* m_model;
	Client* m_client;

	void CreateBucket();
};

#endif
