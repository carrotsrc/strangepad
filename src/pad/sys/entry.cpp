#include <QApplication>
#include <QPushButton>
#include <QFile>
#include "ui/SWindow.hpp"
#include "ui/SHud.hpp"
#include <iostream>

#include "framework/rack/Rack.h"
#include "framework/memory/BitfieldCache.h"


void setupRackoon(RackoonIO::Rack *rack) {
	std::unique_ptr<RackoonIO::RackUnitGenericFactory> factory(new RackoonIO::RackUnitGenericFactory);
	factory->setMessageFactory(new RackoonIO::GenericEventMessageFactory());
	auto cache = new RackoonIO::BitfieldCache();
	cache->init(512, 30);
	factory->setCacheHandler(cache);
	rack->setRackUnitFactory(std::move(factory));
	rack->init();
	rack->initEvents(0);
}

int main(int argc, char **argv)
{
	QApplication app (argc, argv);
	std::cout << "Loading rack.." << std::endl;
	RackoonIO::Rack rack;
	rack.setConfigPath(".config/pad.cfg");
	setupRackoon(&rack);
	rack.start();

	// load style sheet
	QFile qss(".config/strange.qss");
	std::cout << QString(qss.readAll()).toStdString() << std::endl;
	qss.open(QFile::ReadOnly);
	app.setStyleSheet(qss.readAll());

	auto button = new QPushButton();
	button->setText("Hello");

	SWindow window;
	SHud hud("Overview");
	SHud dhud("Focal");

	hud.addWidget(button);
	window.addHeadsup(&hud);
	window.addHeadsup(&dhud);
	window.show();

	return app.exec();
}
