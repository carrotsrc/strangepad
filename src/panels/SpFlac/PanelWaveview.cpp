#include <QLabel>

#include "PanelWaveview.hpp"

#include <QUrl>
#include <QFileInfo>
#include <QMimeData>
#include <iostream>
#include <qt5/QtCore/qiodevice.h>

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

	mui_detail.setText("--- bpm");
	mui_detail.setStyleSheet("font-size: 12px; color: #83C98B; font-family: mono;");
	mui_detailbar.addWidget(&mui_detail);

	mToolbar.addWidget(&mTitle, 0, Qt::AlignLeft|Qt::AlignHCenter);
	mToolbar.addLayout(&mButtonBar);
	

	mContainer.addLayout(&mToolbar);
	mContainer.addLayout(&mui_detailbar);
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
		QFile f(path);
		if(!f.exists()) return;
		
		f.open(QIODevice::ReadOnly);
		char magic[4];
		f.read(magic, 4);
		f.close();
		
		if(strcmp(magic, "fLaC") != 0) return;
		
		mTrackPath = path;
		ru->action_load_file(path.toStdString());
		break;
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
			organise_tags();
			mSampleStep = mWave.getSampleStep();

			mMut.unlock();
			emit update();
		}
		break;
	case SuFlac::streaming:
		mSampleStep = mWave.getSampleStep();
		mPlaying = true;
		emit guiUpdate();
		break;

	case SuFlac::paused:
		mPlaying = false;
		emit guiUpdate();
		break;
		
	case SuFlac::bpm_update:
		if(auto u = unit<SuFlac>()) {
			
			mTrackBpm = u->probe_bpm();
			mTrackBpmLive = u->probe_bpm_live();
			
			this->m_tags.set("bpm", QString::number(mTrackBpm));
			this->m_tags.save();
			
			if(mTrackBpm > 0) {
				
				auto bpm = QString();
				if(mTrackBpm < 100)
					bpm = QString("%1").arg(mTrackBpm, 3, 10, QChar('0'));
				else
					bpm = QString::number(mTrackBpm);
				bpm += " bpm / ";
				bpm += QString::number(mTrackBpmLive);

				mui_detail.setText(bpm);
			} else {
				mui_detail.setText("--- bpm");
			}
		}
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

#include <tag.h>
#include <fileref.h>
#include <tpropertymap.h>
void SpFlacWaveview::organise_tags() {
	QString hash = mWave.getHash();
	std::cout << "Hash: " << hash.toStdString() << std::endl;

	m_tags.load(hash);
	bool modified = false;
	
	if(auto u = unit<SuFlac>()) {
		
		auto sbpm = m_tags.get("bpm");
		if(sbpm != "") {
			u->set_bpm(sbpm.toInt());
			mTrackBpm = sbpm.toInt();
		} else {
			mTrackBpm = 0;
		}

		TagLib::FileRef f(mTrackPath.toStdString().c_str());
		if(f.isNull()) return;
		auto ltag = f.tag();
		
		mTrackArtist = m_tags.get("artist");
		if(mTrackArtist == "") {	
			mTrackArtist = QString(ltag->artist().toCString());
			m_tags.set("artist", mTrackArtist);
			modified = true;
		}
		
		mTrackTitle = m_tags.get("title");
		if(mTrackTitle == "") {	
			mTrackTitle = QString(ltag->title().toCString());
			m_tags.set("title", mTrackTitle);
			modified = true;
		}
		
		mTrackAlbum = m_tags.get("album");
		if(mTrackAlbum == "") {	
			mTrackAlbum = QString(ltag->album().toCString());
			m_tags.set("album", mTrackAlbum);
			modified = true;
		}
		
		if(modified) m_tags.save();
		QString title = QChar(0x25B4)+QString(" ")+mTrackArtist+" - "+mTrackTitle;
		if(mTrackBpm > 0) {
			auto bpm = QString();
			if(mTrackBpm < 100)
				bpm = QString("%1").arg(mTrackBpm, 3, 10, QChar('0'));
			else
				bpm = QString::number(mTrackBpm);
			bpm += " bpm";
			mui_detail.setText(bpm);
		} else {
			mui_detail.setText("--- bpm");
		}
		mTitle.setText(title);
	}
}