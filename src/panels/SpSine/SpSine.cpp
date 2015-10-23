#include <iostream>

#include "SpSine.hpp"
#include "PanelOverview.hpp"

extern "C" QWidget *SpSineBuild(const QString & panelId) {
	if(panelId == "overview") {
		auto obj = new SpSineOverview();
		std::cout << "Classname: " << obj->metaObject()->className() << std::endl;
		return obj;
	}

	return nullptr;
}
