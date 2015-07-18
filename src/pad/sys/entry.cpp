#include <QApplication>
#include <QPushButton>
#include <QFile>
#include <QStack>
#include "ui/SWindow.hpp"
#include "ui/SHud.hpp"
#include <iostream>


#include "framework/rack/Rack.h"
#include "framework/memory/BitfieldCache.h"

#include "ConfigLoader.hpp"
#include "../../panels/ui/SVIndicator.hpp"

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

QVector<SHud*> setupRig(const RigDesc & rig) {
	QVector<SHud*> huds;
	SHud* c = nullptr;

	for(auto it = rig.begin(); it != rig.end(); it++ ) {
		huds.append(new SHud((*it).label));
		for(auto jt = (*it).pads.begin(); jt != (*it).pads.end(); jt++) {
		}
	}

	return huds;
}

int main(int argc, char **argv)
{
	RigDesc rigDescription;

	QApplication app (argc, argv);
	auto sym = libraryTest();


	ConfigLoader configLoader;
	configLoader.load("./.config/pad.xml", &rigDescription);
	auto huds = setupRig(rigDescription);

	RackoonIO::Rack rack;
	rack.setConfigPath(".config/pad.cfg");
	setupRackoon(&rack);
	//rack.start();

	// load style sheet
	QFile qss(".config/strange.qss");
	qss.open(QFile::ReadOnly);
	app.setStyleSheet(qss.readAll());

	SWindow window;
	bool placed = false;

	for(auto hud : huds) {
		if(!placed) {
			auto widget = sym("overview");
			hud->addWidget(widget);
			placed = true;

			widget = sym("overview");
			hud->addWidget(widget);
		}
		window.addHeadsup(hud);
	}

	window.show();

	return app.exec();
}
