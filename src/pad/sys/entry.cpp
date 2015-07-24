#include <QApplication>
#include <QPushButton>
#include <QFile>
#include <QStack>
#include <QLabel>
#include "ui/SWindow.hpp"
#include "ui/SHud.hpp"
#include <iostream>
#include <string>


#include "framework/rack/Rack.h"
#include "framework/memory/BitfieldCache.h"

#include "leveldb/db.h"

#include "ConfigLoader.hpp"
#include "WaveformManager.hpp"

#include "../../panels/ui/SPad.hpp"
#include "../../panels/ui/SSlider.hpp"
#include "../../panels/ui/SKnob.hpp"
#include "../../panels/ui/SWaveform.hpp"

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
	auto libname = QString("SpFlac");
	auto builder = libname + QString("Build");
	auto libPath = QString("./pads/libSpFlac.so");

	QLibrary libLoad(libPath);
	if(!libLoad.load()) {
		std::cout << "Failed to load lib " << libname.toStdString() << std::endl;
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
	rack.start();

	// load style sheet
	QFile qss(".config/strange.qss");
	qss.open(QFile::ReadOnly);
	app.setStyleSheet(qss.readAll());

	SWindow window;
	bool placed = false;
	auto units = rack.getUnits();
	auto testUnit = std::shared_ptr<RackoonIO::RackUnit>(units["flac1"]);

	for(auto hud : huds) {
		if(!placed) {
			auto widget = sym("waveview");
			auto pad = static_cast<SPad*>(widget);
			pad->registerUnit(testUnit);
			auto widgetC = new SKnob();
			auto label =  new QLabel();

			hud->addWidget(pad);
			hud->addWidget(widgetC);
			placed = true;
		}
		window.addHeadsup(hud);
	}
	
	window.show();

	return app.exec();
}
