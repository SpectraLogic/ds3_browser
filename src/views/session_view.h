#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTreeView>
#include <QWidget>

#include "lib/client.h"
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
	Client* m_client;
	Session* m_session;
	QFileSystemModel* m_hostFileSystem;

	QVBoxLayout* m_topLayout;
	QSplitter* m_splitter;
	QTreeView* m_hostBrowser;
	QTreeView* m_remoteBrowser;
};
