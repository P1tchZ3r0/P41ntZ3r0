#include <iostream>
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	MainWindow MW;

	MW.setStyleSheet("QLabel { background-color: #dddddd; }");
	
	MW.show();

	app.exec();
	return 0;	
}
