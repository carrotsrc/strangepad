#include <QApplication>
#include <QPushButton>
#include <QFile>
#include "ui/SWindow.hpp"
#include "ui/SHud.hpp"
#include <iostream>


#include "framework/rack/Rack.h"
#include "framework/memory/BitfieldCache.h"

#include "ConfigLoader.hpp"

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
#include <QLibrary>
typedef QWidget*(*PadBuilder)(const QString &);

PadBuilder libraryTest() {
	auto libname = QString("SpSine");
	auto builder = libname + QString("Build");
	auto libPath = QString("./pads/libSpSine.so");

	QLibrary libLoad(libPath);
	if(!libLoad.load()) {
		std::cout << "Failed to load lib" << std::endl;
		return nullptr;
	}
	auto sym = (PadBuilder) libLoad.resolve(builder.toStdString().c_str());
	if(!sym) {
		std::cout << "failed to resolve builder" << std::endl;
		return nullptr;
	}

	return sym;
}

void initUi() {
	QLibrary local("./pads/libui.so");
	if(!local.load()) {
		std::cout << "Failed to load ui" << std::endl;
	}

	local.setLoadHints(QLibrary::ResolveAllSymbolsHint);
	local.loadHints();

}

void setupRig(const RigDesc & rig) {
	auto it = rig.begin();
	for(;it != rig.end(); it++ ) {
		for(auto jt = (*it).pads.begin(); jt != (*it).pads.end(); jt++) {
			std::cout << "Pad: " << qPrintable((*jt).label) << std::endl;
		}
	}
}

int main(int argc, char **argv)
{
	RigDesc rigDescription;

	ConfigLoader configLoader;
	configLoader.load("./.config/pad.xml", &rigDescription);
	setupRig(rigDescription);

	QApplication app (argc, argv);
	RackoonIO::Rack rack;
	rack.setConfigPath(".config/pad.cfg");
	setupRackoon(&rack);
	//rack.start();

	// load style sheet
	QFile qss(".config/strange.qss");
	qss.open(QFile::ReadOnly);
	app.setStyleSheet(qss.readAll());

	initUi();
	auto sym = libraryTest();
	auto widgetA = sym("overview");
	auto widgetB = sym("overview");
	if(!widgetA || !widgetB) {
		std::cout << "Error loading pad" << std::endl;
		return -1;
	}

	SWindow window;
	SHud hud("Overview");
	SHud dhud("Focal");

	hud.addWidget(widgetA);
	hud.addWidget(widgetB);
	window.addHeadsup(&hud);
	window.addHeadsup(&dhud);
	window.show();

	return app.exec();
}
