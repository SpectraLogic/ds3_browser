#include <QAbstractItemModel>
#include <ds3.h>

class Bucket : public QAbstractItemModel
{
	Q_OBJECT

public:
	Bucket(ds3_get_service_response* response, QObject* parent = 0);
	~Bucket();

	QModelIndex index(int row, int column = 0,
			  const QModelIndex &parent = QModelIndex()) const;

	QModelIndex parent(const QModelIndex &index) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index,
		int role = Qt::DisplayRole) const;

	QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const;

	bool hasChildren(const QModelIndex & parent = QModelIndex()) const;

private:
	ds3_get_service_response* m_get_service_response;

	// Must match COLUMN_NAMES
	enum Column { NAME, OWNER, CREATED, COUNT };
	static const char* const COLUMN_NAMES[];
};
