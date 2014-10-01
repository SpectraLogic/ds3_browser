#ifndef SESSION_VIEW_H
#define SESSION_VIEW_H

#include <QVBoxLayout>
#include <QSplitter>
#include <QWidget>

#include "views/ds3_browser.h"
#include "views/host_browser.h"
#include "models/session.h"

// SessionView, the overall view that is used for each session.  It represents
// the host and DS3 system in a split pane with the host on the left and
// DS3 system on the right.
class SessionView : public QWidget
{
public:
	SessionView(Session* session, QWidget* parent);
	~SessionView();

private:
	DS3Browser* m_ds3Browser;
	HostBrowser* m_hostBrowser;
	Session* m_session;

	QVBoxLayout* m_topLayout;
	QSplitter* m_splitter;
};

#endif
