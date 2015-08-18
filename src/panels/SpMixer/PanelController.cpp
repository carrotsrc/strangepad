#include <QLabel>

#include "PanelController.hpp"

#include <QUrl>
#include <QFileInfo>
#include <QMimeData>
#include <iostream>
SpMixerController::SpMixerController(QWidget *parent) :
SPad(parent) {
	mVgl.addWidget(&mGainLeft);
	auto l = new QLabel("Channel A");
	l->setAlignment(Qt::AlignCenter);
	mVgl.addWidget(l);

	mVgm.addWidget(&mGainMaster);
	l = new QLabel("Master");
	l->setAlignment(Qt::AlignCenter);
	mVgm.addWidget(l);

	mVgr.addWidget(&mGainRight);
	l = new QLabel("Channel B");
	l->setAlignment(Qt::AlignCenter);
	mVgr.addWidget(l);

	mGainBar.addLayout(&mVgl);
	mGainBar.addLayout(&mVgm);
	mGainBar.addLayout(&mVgr);

	//mSplit.setSizeConstraint(QLayout::SetFixedSize);
	mSplit.addLayout(&mGainBar);
	mFader.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	mSplit.addWidget(&mFader);


	mContainer.addWidget(&mLevelsLeft);
	mContainer.addLayout(&mSplit);
	mContainer.addWidget(&mLevelsRight);

	mLevelsLeft.setOrientation(SVIndicator::Left);

	setLayout(&mContainer);
	mProbeTrigger.setParent(this);
	connect(&mProbeTrigger, SIGNAL(timeout()), this, SLOT(probeLevels()));
	mProbeTrigger.start(100);
}

void SpMixerController::onRegisterUnit() {
	if(auto u = unit<SuMixer>()) {
		//u->cbStateChange(mfStateChangePtr);
	}

}

void SpMixerController::probeLevels() {
	auto u = unit<SuMixer>();
	if(!u) return;

	mLevelsLeft.setValue(u->getChannelPeak(0));
	mLevelsRight.setValue(u->getChannelPeak(1));
	emit update();
}
