#include "session.h"

#include "session_dialog.h"

SessionDialog::SessionDialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle("New Spectra Logic DS3 Session");

	m_form = new QFormLayout;

	m_hostLineEdit = new QLineEdit;
	m_hostLineEdit->setToolTip("The name or IP address of the DS3 " \
				   "system's data interface");
	m_form->addRow("BlackPearl Name or IP Address", m_hostLineEdit);

	m_portComboBox = new QComboBox();
	m_portComboBox->addItem("80");
	m_portComboBox->addItem("8080");
	m_portComboBox->setToolTip("The port that the DS3 system's DS3 " \
				   "service is running on (usually 80)");
	m_form->addRow("BlackPearl DS3 Port", m_portComboBox);

	m_accessIdLineEdit = new QLineEdit;
	m_form->addRow("S3 Access ID", m_accessIdLineEdit);

	m_secretKeyLineEdit = new QLineEdit;
	m_form->addRow("S3 Secret Key", m_secretKeyLineEdit);

	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
					   | QDialogButtonBox::Cancel);

	connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(Accept()));
	connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(Reject()));

	m_layout = new QVBoxLayout;
	m_layout->addLayout(m_form);
	m_layout->addWidget(m_buttonBox);

	setLayout(m_layout);

	m_session = new Session;
}

SessionDialog::~SessionDialog()
{
	delete m_session;
}

void
SessionDialog::Accept()
{
	UpdateSession();
	accept();
}

void
SessionDialog::Reject()
{
	reject();
}

void
SessionDialog::UpdateSession()
{
	m_session->SetHost(m_hostLineEdit->text().toUtf8().constData());
	m_session->SetPort(m_portComboBox->currentText().toInt());
	m_session->SetAccessId(m_accessIdLineEdit->text().toUtf8().constData());
	m_session->SetSecretKey(m_secretKeyLineEdit->text().toUtf8().constData());
}
