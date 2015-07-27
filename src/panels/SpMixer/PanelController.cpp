#include <QLabel>

#include "PanelController.hpp"

#include <QUrl>
#include <QFileInfo>
#include <QMimeData>
#include <iostream>
SpMixerController::SpMixerController(QWidget *parent) :
SPad(parent) {
	mGainBar.addWidget(&mGainLeft);
	mGainBar.addWidget(&mGainRight);

	mContainer.addWidget(&mLevelsLeft);
	mContainer.addLayout(&mGainBar);
	mContainer.addWidget(&mLevelsRight);

	mLevelsRight.setValue(0.8);
	mLevelsLeft.setValue(0.3);
	mLevelsLeft.setOrientation(SVIndicator::Left);

	setLayout(&mContainer);
}

void SpMixerController::onRegisterUnit() {
	if(auto u = unit<SuMixer>()) {
//		u->cbStateChange(mfStateChangePtr);
	}

}
