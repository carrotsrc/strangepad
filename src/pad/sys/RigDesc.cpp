#include "RigDesc.hpp"
#include <iostream>
void RigDesc::addHud(QString label) {
	std::cout << "Added hud: " << qPrintable(label) << std::endl;
	auto ptr = new HudDesc {
		.label = label
	};

	mHuds.push_back(ptr);
	mCurrentHud = ptr;
}
