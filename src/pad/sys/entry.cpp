#include <iostream>
#include <string>

#include <QApplication>
#include <QPushButton>
#include <QFile>
#include <QLabel>
#include <QLibrary>

#include "ui/SWindow.hpp"

#include "setup.hpp"

int main(int argc, char **argv)
{
	RigDesc rigDescription;
	PadLoader padLoader;

	QApplication app (argc, argv);

	ConfigLoader configLoader;
	configLoader.load("./.config/pad.xml", &rigDescription);

	RackoonIO::Rack rack;
	rack.setConfigPath(".config/pad.cfg");
	setupRackoon(&rack);

	auto huds = setupRig(rigDescription, &padLoader, &rack);

	rack.start();

	// load style sheet
	QFile qss(".config/strange.qss");
	qss.open(QFile::ReadOnly);
	app.setStyleSheet(qss.readAll());

	SWindow window;
	for(auto hud : huds) {
		window.addHeadsup(hud);
	}
	
	window.show();

	return app.exec();
}
