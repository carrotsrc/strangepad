#include <iostream>

#include "SpSine.hpp"
#include "PanelOverview.hpp"

extern "C" QWidget *SpSineBuild(const QString & panelId) {
	std::cout << "Loading " << panelId.toStdString() << std::endl;
	if(panelId == "overview") {
		return new SpSineOverview();
	}

	return nullptr;
}
