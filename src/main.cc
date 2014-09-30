#include <QApplication>

#include "main_window.h"

int
main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Spectra Logic");
	app.setApplicationName("DS3 Explorer");

	MainWindow mainWindow;
	if (mainWindow.IsFinished())
	{
		// User closed/cancelled the New Session dialog
		return 0;
	}
	mainWindow.show();

	return app.exec();
}
