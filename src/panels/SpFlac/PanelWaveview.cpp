#include <QLabel>

#include "PanelWaveview.hpp"

#include <iostream>
SpFlacWaveview::SpFlacWaveview(QWidget *parent) :
SPad(parent) {
	mfStateChangePtr.reset(new SuflCbStateChange(
				std::bind(&SpFlacWaveview::onUnitStateChange, this, std::placeholders::_1)
				));

	mContainer.addWidget(&mWave);
	setLayout(&mContainer);

}

void SpFlacWaveview::onRegisterUnit() {
	if(auto u = unit<SuFlacLoad>()) {
		u->cbStateChange(mfStateChangePtr);
	}

}

void SpFlacWaveview::onUnitStateChange(SuFlacLoad::WorkState state) {
	switch(state) {
	case SuFlacLoad::PRESTREAM:
		if(auto u = unit<SuFlacLoad>()) {
			mWave.setWaveData(u->getSampleData(), u->getSpc());
		}
		break;
	default:
		break;
	}
}
