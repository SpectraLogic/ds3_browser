#ifndef SESSION_DIALOG_H
#define SESSION_DIALOG_H

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include "models/session.h"

// SessionDialog, a dialog to gather host/user information when creating
// a new host<->DS3 System session.
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

	QLabel* m_hostLabel;
	QLineEdit* m_hostLineEdit;
	QComboBox* m_portComboBox;
	QLabel* m_accessIdLabel;
	QLineEdit* m_accessIdLineEdit;
	QLabel* m_secretKeyLabel;
	QLineEdit* m_secretKeyLineEdit;

	QDialogButtonBox* m_buttonBox;

	Session* m_session;

	void ValidateLineEditNotEmpty(QLabel*, QLineEdit*, bool&);

	void UpdateSession();
};

inline Session*
SessionDialog::GetSession()
{
	return m_session;
}

#endif
