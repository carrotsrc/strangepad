#include <QApplication>
#include <QPushButton>
#include <QFile>
#include <QStack>
#include <QLabel>
#include <QLibrary>

#include "ui/SWindow.hpp"
#include "ui/SHud.hpp"
#include <iostream>
#include <string>


#include "framework/rack/Rack.h"
#include "framework/memory/BitfieldCache.h"

#include "leveldb/db.h"

#include "ConfigLoader.hpp"
#include "PadLoader.hpp"
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

QVector<SHud*> setupRig(const RigDesc & rig) {
	QVector<SHud*> huds;
	SHud* c = nullptr;

	for(auto it = rig.begin(); it != rig.end(); it++ ) {
		huds.append(new SHud((*it).label));
		for(auto jt : (*it).pads) {
			std::cout << jt.collection.toStdString() << std::endl;
		}
	}

	return huds;
}


int main(int argc, char **argv)
{
	RigDesc rigDescription;
	PadLoader padLoader;

	QApplication app (argc, argv);

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
			auto pad = padLoader.load("SpFlac", "waveview");
			pad->registerUnit(testUnit);
			auto widgetC = new SKnob();
			auto label =  new QLabel();

			hud->addWidget(pad.release());
			hud->addWidget(widgetC);
			placed = true;
		}
		window.addHeadsup(hud);
	}
	
	window.show();

	return app.exec();
}
