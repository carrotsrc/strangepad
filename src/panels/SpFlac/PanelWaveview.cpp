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

	mPause.setText(QString(QChar(0x25AE))+QString(QChar(0x25AE)));
	mPause.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	mButtonBar.addWidget(&mPlay);
	mButtonBar.addWidget(&mPause);

	mContainer.addWidget(&mWave);

	mToolbar.addWidget(&mTitle, 0, Qt::AlignLeft|Qt::AlignHCenter);
	mToolbar.addLayout(&mButtonBar);

	mContainer.addLayout(&mToolbar);

	setLayout(&mContainer);

	setAcceptDrops(true);
}

void SpFlacWaveview::onRegisterUnit() {
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
		std::cout << "Loading...." << std::endl;
		mTitle.setText("Loading...");
		emit update();
		break;
	case SuFlacLoad::PRESTREAM:
		if(auto u = unit<SuFlacLoad>()) {
			mWave.setWaveData(u->getSampleData(), u->getSpc());
			auto info = QFileInfo(QString(u->getFilename().c_str()));
			mTitle.setText(QChar(0x25B4)+QString(" ")+info.baseName());
			emit update();
		}
		break;
	default:
		break;
	}
}
