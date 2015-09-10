#include <QLabel>

#include "PanelController.hpp"

#include <QUrl>
#include <QFileInfo>
#include <QMimeData>
#include <iostream>
SpMixerController::SpMixerController(QWidget *parent) :
SPad(parent) {
	m_shr_onchange.reset( new sumixer_onchange_cb(
				std::bind(&SpMixerController::onGainChange, this, 
					std::placeholders::_1, 
					std::placeholders::_2)
			)
	);

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

	mSplit.setSizeConstraint(QLayout::SetFixedSize);
	mSplit.addLayout(&mGainBar);
	mFader.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	mFader.setStart(64);
	//mSplit.addWidget(&mFader);


	mLevelsLeft.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	mLevelsLeft.setOrientation(SLevel::Left);
	mLevelsRight.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	mContainer.addWidget(&mLevelsLeft);
	mContainer.addLayout(&mSplit);
	mContainer.addWidget(&mLevelsRight);

	setLayout(&mContainer);
	mProbeTrigger.setParent(this);
	connect(&mProbeTrigger, SIGNAL(timeout()), this, SLOT(probeLevels()));
	mProbeTrigger.start(100);
}

void SpMixerController::onRegisterUnit() {
	if(auto u = unit<SuMixer>()) {
		u->listen_onchange(m_shr_onchange);
	}

}

void SpMixerController::probeLevels() {
	
	auto u = unit<SuMixer>();
	if(!u) return;

	mLevelsLeft.setValue(u->probe_channel_peak(1));
	mLevelsRight.setValue(u->probe_channel_peak(0));

	emit update();
}

void SpMixerController::onGainChange(SuMixer::gain_type type, int value) {
		switch(type) {
			case SuMixer::channel_a: mGainLeft.setValue(value); break;
			case SuMixer::channel_b: mGainRight.setValue(value); break;
			case SuMixer::master: mGainMaster.setValue(value); break;
			//case SuMixer::Fader: mFader.setValue(value); mFader.update(); break;
		}
}
