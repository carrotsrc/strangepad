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
	QString configPath = ".config/pad.xml";
	for(auto i = 0; i < argc; i++) {
		if(strcmp(argv[i], "-c") == 0 && argc > 1) {
			configPath = QString(argv[i+1]);
		}
	}

	ConfigLoader configLoader;
	configLoader.load(configPath, &rigDescription);

	RackoonIO::Rack rack;
	std::cout << "Loading rack: " << rigDescription.getRackConfig().toStdString() << std::endl;
	rack.setConfigPath(rigDescription.getRackConfig().toStdString());
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
