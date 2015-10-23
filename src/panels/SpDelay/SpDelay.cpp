#include <iostream>

#include "SpDelay.hpp"
#include "ControllerPanel.hpp"

extern "C" QWidget *SpDelayBuild(const QString & panelId) {
	if(panelId == "controller") {
		auto obj = new SpDelayController();
		return obj;
	}


	return nullptr;
}
