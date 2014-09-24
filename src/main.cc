#include <QApplication>

#include "main_window.h"

int
main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Spectra Logic");
	app.setApplicationName("DS3 Explorer");

	MainWindow mainWindow;
	mainWindow.show();

	return app.exec();
}
