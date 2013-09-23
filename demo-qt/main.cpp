/*
 * demo-qt5/main.cpp: demonstration of the AcousticAVE library with Qt5
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <QApplication>
#include "view.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("AcousticAVE");
	View view;
	view.show();
	return app.exec();
}
