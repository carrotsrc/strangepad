#include <iostream>

#include "SpMixer.hpp"
#include "PanelController.hpp"

extern "C" QWidget *SpMixerBuild(const QString & panelId) {
	if(panelId == "controller") {
		auto obj = new SpMixerController();
		return obj;
	}


	return nullptr;
}
