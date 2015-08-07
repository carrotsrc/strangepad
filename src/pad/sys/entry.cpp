#include <iostream>
#include <string>

#include <QApplication>
#include <QPushButton>
#include <QFile>
#include <QLabel>
#include <QLibrary>

#include "ui/SWindow.hpp"

#include "setup.hpp"

static int branchNoGui(StrangeIO::Rack *rack);
int main(int argc, char **argv)
{
	RigDesc rigDescription;
	PadLoader padLoader;
	bool loadGui = true;

	QString configPath = ".config/pad.xml";
	for(auto i = 0; i < argc; i++) {
		if(strcmp(argv[i], "-c") == 0 && argc > 1) {
			configPath = QString(argv[i+1]);
		} else if(strcmp(argv[i], "--no-gui") == 0) {
			loadGui = false;
			std::cout << "StrangePad: GUI Disabled" << std::endl;
		}
	}

	ConfigLoader configLoader;
	configLoader.load(configPath, &rigDescription);

	StrangeIO::Rack rack;
	std::cout << "Loading rack: " << rigDescription.getRackConfig().toStdString() << std::endl;
	rack.setConfigPath(rigDescription.getRackConfig().toStdString());
	setupRackoon(&rack);

	QApplication app (argc, argv);
	auto huds = setupRig(rigDescription, &padLoader, &rack);

	if(!loadGui) {
		return branchNoGui(&rack);
	}


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

static int branchNoGui(StrangeIO::Rack *rack) {
	bool running = true;
	std::string in;
	rack->start();
	while(running) {
		std::cout << "rack@strangepad $ " << std::flush;
		std::cin >> in;
		if(in == "quit")
			running = false;
	}

	return 0;
}

