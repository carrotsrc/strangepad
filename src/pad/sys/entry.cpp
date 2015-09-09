#include <iostream>
#include <string>

#include <QApplication>
#include <QPushButton>
#include <QFile>
#include <QLabel>
#include <QLibrary>

#include "framework/routine/system.hpp"

#include "ui/SWindow.hpp"

#include "setup.hpp"

static int branchNoGui(siocom::rack *srack);
int main(int argc, char **argv)
{
	RigDesc rigDescription;
	PadLoader padLoader;
	siocfg::assembler as;
	
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


	std::cout << "Loading rack: " << rigDescription.getRackConfig().toStdString() << std::endl;
	auto srack = siortn::system::setup(as, rigDescription.getRackConfig().toStdString(), 64);

	QApplication app (argc, argv);
	auto huds = setupRig(rigDescription, &padLoader, srack.get());

	if(!loadGui) {
		return branchNoGui(srack.release());
	}

	srack->warmup();
	srack->start();

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


static int branchNoGui(siocom::rack *srack) {
	bool running = true;
	std::string in;
	srack->warmup();
	srack->start();
	while(running) {
		std::cout << "rack@strangepad $ " << std::flush;
		std::cin >> in;
		if(in == "quit")
			running = false;
	}
	delete srack;
	return 0;
}