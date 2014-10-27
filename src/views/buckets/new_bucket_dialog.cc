#include "lib/client.h"
#include "views/buckets/new_bucket_dialog.h"

NewBucketDialog::NewBucketDialog(Client* client, QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle("New Bucket");

	m_client = client;

	m_form = new QFormLayout;

	m_bucketLineEdit = new QLineEdit;
	m_bucketLineEdit->setToolTip("The name of the Bucket to create");
	m_bucketLabel = new QLabel("Bucket Name");
	m_form->addRow(m_bucketLabel, m_bucketLineEdit);

	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
					   | QDialogButtonBox::Cancel);

	connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(Accept()));
	connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(Reject()));

	m_layout = new QVBoxLayout;
	m_layout->addLayout(m_form);
	m_layout->addWidget(m_buttonBox);

	setLayout(m_layout);
}

void
NewBucketDialog::Accept()
{
	bool valid = true;
	ValidateLineEditNotEmpty(m_bucketLabel, m_bucketLineEdit, valid);
	if (!valid) {
		return;
	}

	UpdateBucket();
	CreateBucket();
	accept();
}

void
NewBucketDialog::ValidateLineEditNotEmpty(QLabel* label, QLineEdit* lineEdit, bool& valid)
{
	if (lineEdit->text().trimmed().isEmpty()) {
		label->setStyleSheet("QLabel { color: red; }");
		valid = false;
	} else {
		label->setStyleSheet("");
	}
}

void
NewBucketDialog::Reject()
{
	reject();
}

void
NewBucketDialog::UpdateBucket()
{
	m_bucket = m_bucketLineEdit->text().trimmed();
}

void
NewBucketDialog::CreateBucket()
{
	m_client->CreateBucket(m_bucket.toUtf8().constData());
}
