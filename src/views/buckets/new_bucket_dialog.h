#ifndef NEW_BUCKET_DIALOG_H
#define NEW_BUCKET_DIALOG_H

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

class Client;

class NewBucketDialog : public QDialog
{
	Q_OBJECT

public:
	NewBucketDialog(Client* client, QWidget* parent = 0);

	const QString& GetBucket();

public slots:
	void Accept();
	void Reject();

private:
	QVBoxLayout* m_layout;

	QFormLayout* m_form;

	QLabel* m_bucketLabel;
	QLineEdit* m_bucketLineEdit;

	QDialogButtonBox* m_buttonBox;

	Client* m_client;

	QString m_bucket;

	void ValidateLineEditNotEmpty(QLabel*, QLineEdit*, bool&);

	void UpdateBucket();
	void CreateBucket();
};

inline const QString&
NewBucketDialog::GetBucket()
{
	return m_bucket;
}

#endif
