#include <QApplication>
#include <QPushButton>
#include <QFile>
#include <QStack>
#include <QLabel>
#include "ui/SWindow.hpp"
#include "ui/SHud.hpp"
#include <iostream>


#include "framework/rack/Rack.h"
#include "framework/memory/BitfieldCache.h"

#include "leveldb/db.h"

#include "ConfigLoader.hpp"
#include "WaveformManager.hpp"

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

signed short *loadWave(const QString & path, long long* len) { 
	QFile waveFile(path);
	waveFile.open(QFile::ReadOnly);
	auto sz = waveFile.size();

	*len = sz/4;

	auto wave = (signed short*)malloc(sizeof(signed short)*sz);
	auto read = waveFile.read((char*)wave, sz);

	return wave;
}

void generateWaveform() {
	
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


	long long waveLength = 0;
	auto wave = loadWave("/home/charlie/Spatialize.wav", &waveLength);

	WaveformManager wfManager;
	wfManager.generate(640,160, wave, waveLength);
//	wfManager.setRaw(wave, waveLength);
	//auto hash = waveform.hash();
//	wfManager.minCompression();
//	auto img = wfManager.generate(860,200);



	// load style sheet
	QFile qss(".config/strange.qss");
	qss.open(QFile::ReadOnly);
	app.setStyleSheet(qss.readAll());

	SWindow window;
	bool placed = false;

	for(auto hud : huds) {
		if(!placed) {
		//	auto widget = sym("overview");
			auto widget = new SSlider;
			auto widgetB = new SWaveform();
			auto widgetC = new SKnob();
			auto label =  new QLabel();
			//label->setPixmap(QPixmap::fromImage(img));
			//label->setAttribute(Qt::WA_TranslucentBackground);

			widgetB->setWaveData(wave, waveLength);
			hud->addWidget(widget);
			hud->addWidget(widgetB);
			hud->addWidget(widgetC);
			hud->addWidget(label);
			placed = true;
		}
		window.addHeadsup(hud);
	}
	
	window.show();

	return app.exec();
}
