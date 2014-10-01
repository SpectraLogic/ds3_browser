#ifndef DS3_BROWSER_H
#define DS3_BROWSER_H

#include "lib/client.h"
#include "models/session.h"
#include "models/bucket.h"
#include "views/browser.h"

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
	void OnModelItemDoubleClick(const QModelIndex& index);

private:
	Bucket* m_model;
	Client* m_client;
};

#endif
