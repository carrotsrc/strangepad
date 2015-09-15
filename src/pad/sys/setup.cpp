#include "setup.hpp"

using namespace strangeio;



QVector<SHud*> setupRig(const RigDesc & rig, PadLoader *padLoader, siocom::rack *sys) {
	QVector<SHud*> huds;

	for(auto it = rig.begin(); it != rig.end(); it++ ) {
		auto hud = new SHud((*it).label);
		for(auto jt : (*it).pads) {
			auto pad = padLoader->load(jt.collection, jt.type);
			if(pad == nullptr) {
				continue;
			}
			auto wptr = sys->get_unit(jt.unit.toStdString());
			if(wptr.expired()) continue;
			pad->registerUnit(wptr);

			hud->addWidget(pad.release());
		}
		huds.append(hud);
	}

	return huds;
}

