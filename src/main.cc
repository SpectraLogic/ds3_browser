#include <QApplication>

#include "main_window.h"
#include "models/session.h"
#include "views/session_dialog.h"

int
main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Spectra Logic");
	app.setApplicationName("DS3 Explorer");

	SessionDialog* sessionDialog = new SessionDialog;
	if (sessionDialog->exec() == QDialog::Rejected) {
		return 0;
	}
	Session* session = new Session(*sessionDialog->GetSession());
	delete sessionDialog;

	MainWindow mainWindow(session);
	mainWindow.show();

	return app.exec();
}
