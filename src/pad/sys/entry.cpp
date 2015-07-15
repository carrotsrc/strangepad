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
#include <QLibrary>

QWidget *libraryTest() {
	QLibrary local("./pads/libui.so");
	if(!local.load()) {
		std::cout << "Failed to load ui" << std::endl;
		return nullptr;
	}

	local.setLoadHints(QLibrary::ResolveAllSymbolsHint);
	local.loadHints();

	auto libname = QString("SpSine");
	auto builder = libname + QString("Build");
	auto libPath = QString("./pads/libSpSine.so");

	QLibrary libLoad(libPath);
	if(!libLoad.load()) {
		std::cout << "Failed to load lib" << std::endl;
		return nullptr;
	}
	typedef QWidget*(*PadBuilder)(const QString &);
	auto sym = (PadBuilder) libLoad.resolve(builder.toStdString().c_str());
	if(!sym) {
		std::cout << "failed to resolve builder" << std::endl;
	}
	return sym("overview");

}

int main(int argc, char **argv)
{
	QApplication app (argc, argv);
	RackoonIO::Rack rack;
	rack.setConfigPath(".config/pad.cfg");
	setupRackoon(&rack);
	//rack.start();

	// load style sheet
	QFile qss(".config/strange.qss");
	qss.open(QFile::ReadOnly);
	app.setStyleSheet(qss.readAll());

	auto widget = libraryTest();
	if(!widget) {
		std::cout << "Error loading pad" << std::endl;
		return -1;
	}

		std::cout << "Check: " << widget->metaObject()->className() << std::endl;
	SWindow window;
	SHud hud("Overview");
	SHud dhud("Focal");

	hud.addWidget(widget);
	window.addHeadsup(&hud);
	window.addHeadsup(&dhud);
	window.show();

	return app.exec();
}
