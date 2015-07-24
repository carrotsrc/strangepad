#include <QLabel>

#include "PanelWaveview.hpp"

#include <iostream>
SpFlacWaveview::SpFlacWaveview(QWidget *parent) :
SPad(parent) {
	mfStateChangePtr.reset(new SuflCbStateChange(
				std::bind(&SpFlacWaveview::onUnitStateChange, this, std::placeholders::_1)
				));
	
}

void SpFlacWaveview::onRegisterUnit() {
	if(auto u = unit<SuFlacLoad>()) {
		std::cout << "Name: " << u->getFilename() << std::endl;
	}

}

void SpFlacWaveview::onUnitStateChange(SuFlacLoad::WorkState state) {
	std::cout << "Workstate: " << int(state) << std::endl;
}
