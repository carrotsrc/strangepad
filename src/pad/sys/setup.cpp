#include "setup.hpp"
#include "framework/memory/BitfieldCache.h"

void setupRackoon(RackoonIO::Rack *rack) {
	std::unique_ptr<RackoonIO::RackUnitGenericFactory> factory(new RackoonIO::RackUnitGenericFactory);
	factory->setMessageFactory(new RackoonIO::GenericEventMessageFactory());
	auto cache = new RackoonIO::BitfieldCache();
	cache->init(512, 32);
	factory->setCacheHandler(cache);
	rack->setRackUnitFactory(std::move(factory));
	rack->init();
	rack->initEvents(0);
}

QVector<SHud*> setupRig(const RigDesc & rig, PadLoader *padLoader, RackoonIO::Rack *rack) {
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
