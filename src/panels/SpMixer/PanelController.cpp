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

	mLevelsLeft.setOrientation(SVIndicator::Left);

	setLayout(&mContainer);
	mProbeTrigger.setParent(this);
	connect(&mProbeTrigger, SIGNAL(timeout()), this, SLOT(probeLevels()));
	mProbeTrigger.start(250);
}

void SpMixerController::onRegisterUnit() {
	if(auto u = unit<SuMixer>()) {
//		u->cbStateChange(mfStateChangePtr);
	}

}

void SpMixerController::probeLevels() {
	auto u = unit<SuMixer>();
	if(!u) return;

	mLevelsLeft.setValue(u->getChannelPeak(0));
	mLevelsRight.setValue(u->getChannelPeak(1));
}
