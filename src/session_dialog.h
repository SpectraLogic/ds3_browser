#ifndef SESSION_DIALOG_H
#define SESSION_DIALOG_H

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include "session.h"

class SessionDialog : public QDialog
{
	Q_OBJECT

public:
	SessionDialog(QWidget* parent = 0);
	~SessionDialog();

	Session* GetSession();

public slots:
	void Accept();
	void Reject();

private:

	QVBoxLayout* m_layout;

	QFormLayout* m_form;

	QLineEdit* m_hostLineEdit;
	QComboBox* m_portComboBox;
	QLineEdit* m_accessIdLineEdit;
	QLineEdit* m_secretKeyLineEdit;

	QDialogButtonBox* m_buttonBox;

	Session* m_session;

	void UpdateSession();
};

inline Session*
SessionDialog::GetSession()
{
	return m_session;
}

#endif
