#include <iostream>

#include "SpFlac.hpp"
#include "PanelWaveview.hpp"

extern "C" QWidget *SpFlacBuild(const QString & panelId) {
	if(panelId == "waveview") {
		auto obj = new SpFlacWaveview();
		return obj;
	}

	return nullptr;
}
