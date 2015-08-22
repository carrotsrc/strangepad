#include "setup.hpp"
#include "framework/memory/BitfieldCache.h"
#include "framework/rack/config/RackDocument.h"
#include "framework/rack/config/RackAssembler.h"
using namespace StrangeIO;

static void setupFactory(RackUnitGenericFactory* factory) {

	factory->setMessageFactory(new StrangeIO::GenericEventMessageFactory());
	auto cache = new StrangeIO::BitfieldCache();
	cache->init(512, 64);
	factory->setCacheHandler(cache);
}

void setupIo(StrangeIO::Rack *rack, QString path) {
	Config::RackDocument doc;
	rack->setRackQueue(std::unique_ptr<RackQueue>(new RackQueue(0)));

	std::unique_ptr<StrangeIO::RackUnitGenericFactory> factory(new StrangeIO::RackUnitGenericFactory);
	setupFactory(factory.get());

	auto config = doc.load(path.toStdString());
	Config::RackAssembler as(std::move(factory));
	as.assemble((*config), (*rack));
	rack->init();
	rack->initEvents(0);
}

QVector<SHud*> setupRig(const RigDesc & rig, PadLoader *padLoader, StrangeIO::Rack *rack) {
	QVector<SHud*> huds;

	for(auto it = rig.begin(); it != rig.end(); it++ ) {
		auto hud = new SHud((*it).label);
		for(auto jt : (*it).pads) {
			auto pad = padLoader->load(jt.collection, jt.type);
			if(pad == nullptr) {
				continue;
			}
			pad->registerUnit(rack->getUnit(jt.unit.toStdString()));

			hud->addWidget(pad.release());
		}
		huds.append(hud);
	}

	return huds;
}
