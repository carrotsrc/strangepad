#include <QLabel>

#include "PanelWaveview.hpp"

#include <QUrl>
#include <QFileInfo>
#include <QMimeData>
#include <iostream>
SpFlacWaveview::SpFlacWaveview(QWidget *parent) :
SPad(parent) {
	mfStateChangePtr.reset(new SuflCbStateChange(
				std::bind(&SpFlacWaveview::onUnitStateChange, this, std::placeholders::_1)
				));


	mTitle.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	mTitle.setStyleSheet("font-size: 18px;");
	mTitle.setText(QString(QChar(0x25B4))+QString(" Idle"));

	mPlay.setText(QChar(0x25B6));
	mPlay.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	connect(&mPlay, SIGNAL(clicked()), this, SLOT(triggerMidiPlay()));

	mPause.setText(QString(QChar(0x25AE))+QString(QChar(0x25AE)));
	mPause.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	mButtonBar.addWidget(&mPlay);
	mButtonBar.addWidget(&mPause);

	mContainer.addWidget(&mWave);

	mToolbar.addWidget(&mTitle, 0, Qt::AlignLeft|Qt::AlignHCenter);
	mToolbar.addLayout(&mButtonBar);

	mContainer.addLayout(&mToolbar);

	connect(&mProgressTrigger, SIGNAL(timeout()), this, SLOT(probeProgress()));
	mProgressTrigger.start(250);

	setLayout(&mContainer);

	setAcceptDrops(true);

	mPlaying = false;
}

void SpFlacWaveview::onRegisterUnit() {
	connect(this, SIGNAL(guiUpdate()), this, SLOT(onGuiUpdate()));
	if(auto u = unit<SuFlacLoad>()) {
		u->cbStateChange(mfStateChangePtr);
	}

}

void SpFlacWaveview::dragEnterEvent(QDragEnterEvent *e) {

	e->acceptProposedAction();
	mWave.toggleBgHighlight(true);
	update();
}

void SpFlacWaveview::dragLeaveEvent(QDragLeaveEvent *e) {
	e->accept();
	mWave.toggleBgHighlight(false);
	update();
}
void SpFlacWaveview::dragMoveEvent(QDragMoveEvent *e) {
	e->acceptProposedAction();
}

void SpFlacWaveview::dropEvent(QDropEvent *e) {
	e->acceptProposedAction();
	mWave.toggleBgHighlight(false);

	const auto mimeData = e->mimeData();
	auto ru = unit<SuFlacLoad>();

	for(auto url : mimeData->urls()) {
		auto path = url.path();
		QFileInfo info(path);
		if(info.completeSuffix() != "flac") {
			return;
			update();
		}

		ru->setFilename(path.toStdString());
	}
	update();
	ru->init();
	update();
}


void SpFlacWaveview::onUnitStateChange(SuFlacLoad::WorkState state) {
	switch(state) {
	case SuFlacLoad::LOADING:
		mMut.lock();
		mTitle.setText("Loading...");
		mMut.unlock();
		emit update();
		break;
	case SuFlacLoad::PRESTREAM:
		if(auto u = unit<SuFlacLoad>()) {
			mMut.lock();
			mWave.setWaveData(u->getSampleData(), u->getSpc());
			mSampleStep = mWave.getSampleStep();
			auto info = QFileInfo(QString(u->getFilename().c_str()));
			mTitle.setText(QChar(0x25B4)+QString(" ")+info.baseName());
			emit update();
			mMut.unlock();
		}
		break;
	case SuFlacLoad::STREAMING:
		mPlaying = true;
		emit guiUpdate();
		break;

	case SuFlacLoad::PAUSED:
		mPlaying = false;
		emit guiUpdate();
		break;
	default:
		break;
	}

}

void SpFlacWaveview::triggerMidiPlay() {
	if(auto u = unit<SuFlacLoad>()) {
		u->midiPause(127);
	}
}

void SpFlacWaveview::onGuiUpdate() {
	if(mPlaying) {
		mPlay.setStyleSheet("color: #F97FFF;");
	} else {
		mPlay.setStyleSheet("color: #8E06A0;");
	}
}

void SpFlacWaveview::probeProgress() {
	if(auto u = unit<SuFlacLoad>()) {
		auto prog = u->getProgress()/2;
		mNextStep -= prog;
		mWave.updateProgress(prog);
		if(mNextStep <= 0) {
			emit guiUpdate();
			mNextStep = mSampleStep;
		}
	}
}
