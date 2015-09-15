#include <QLabel>

#include "PanelWaveview.hpp"

#include <QUrl>
#include <QFileInfo>
#include <QMimeData>
#include <iostream>
SpFlacWaveview::SpFlacWaveview(QWidget *parent) :
SPad(parent) {
	
	mOnChangePtr.reset(new suflac_onchange_cb(
				std::bind(&SpFlacWaveview::listenOnChange, this, std::placeholders::_1)
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
	if(auto u = unit<SuFlac>()) {
		u->listen_onchange(mOnChangePtr);
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
	auto ru = unit<SuFlac>();

	for(auto url : mimeData->urls()) {
		auto path = url.path();
		QFileInfo info(path);
		if(info.completeSuffix() != "flac") {
			return;
			update();
		}

		ru->action_load_file(path.toStdString());
	}
	update();
}

void SpFlacWaveview::listenOnChange(SuFlac::working_state state) {
	switch(state) {
	case SuFlac::loading:
		mMut.lock();
		mTitle.setText("Loading...");
		mMut.unlock();
		emit update();
		break;
	case SuFlac::prestream:
		if(auto u = unit<SuFlac>()) {
			mMut.lock();
			mWave.setWaveData(u->probe_flac_data(), u->probe_total_spc());
			mSampleStep = mWave.getSampleStep();
			auto info = QFileInfo(QString(u->probe_flac_path().c_str()));
			mTitle.setText(QChar(0x25B4)+QString(" ")+info.baseName());
			mMut.unlock();
			emit update();
		}
		break;
	case SuFlac::streaming:
		mPlaying = true;
		emit guiUpdate();
		break;

	case SuFlac::paused:
		mPlaying = false;
		emit guiUpdate();
		break;

	default:
		break;
	}

}

void SpFlacWaveview::triggerMidiPlay() {
	if(auto u = unit<SuFlac>()) {
		u->action_start_stream();
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
	if(auto u = unit<SuFlac>()) {


		auto prog = u->probe_progress();
		mNextStep -= prog;
		mWave.updateProgress(prog);
		if(mNextStep <= 0) {
			emit guiUpdate();
			mNextStep = mSampleStep;
		}

	}
}

void SpFlacWaveview::mouseReleaseEvent(QMouseEvent* e) {
	auto wrect = mWave.geometry();
	if(e->button() == Qt::LeftButton && mWave.rect().contains(e->pos())) {
		auto x = e->x() - mWave.x();
		auto jump = x * mWave.getSampleStep();
		if(auto u = unit<SuFlac>()) {
			u->action_jump_to_sample(jump);
		}
	}
}
