#ifndef BROWSER_H
#define BROWSER_H

#include <QAction>
#include <QLabel>
#include <QToolbar>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>

// Browser, an abstract class that represents a host or remote file browser
class Browser : public QWidget
{
	Q_OBJECT

public:
	Browser(QWidget* parent = 0, Qt::WindowFlags flags = 0);
	virtual ~Browser();

protected:
	QVBoxLayout* m_layout;
	QLabel* m_path;
	QToolBar* m_toolBar;
	QTreeView* m_treeView;

	QAction* m_parentAction;
	QAction* m_rootAction;

	void AddToolBarActions();
	virtual void UpdatePathLabel(const QString& path);

protected slots:
	virtual void GoToParent() = 0;
	virtual void GoToRoot() = 0;
	virtual void OnModelItemDoubleClick(const QModelIndex& index) = 0;
};

#endif
