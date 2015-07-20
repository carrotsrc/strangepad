#include <QPainter>
#include <QMouseEvent>
#include <qmath.h>
#include "SWaveform.hpp"
#include <iostream>
SWaveform::SWaveform(QWidget* parent)
: QWidget(parent) {
	mWaveData = nullptr;
	mHoverPosition = -1;
	setMouseTracking(true);
}

void SWaveform::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing);

	QPen pen(QColor("#ffffff"));
	pen.setWidth(1);

	QRectF mWaveRect(0,0, width(), height());

	painter.setPen(pen);

	if(mWaveData == nullptr) return;
	auto step = mWaveLength/(mWaveRect.width()-10);
	mSampleStep = step;
	auto centre = mWaveRect.height()/2;

	signed short max = 32768;
	signed short min = -32767;

	auto stepHi = (mWaveRect.height())/max;
	auto stepLo = (mWaveRect.height())/min;


	auto x = 0, y = 0;
	auto sampleIndex = 0;
	signed short sample;

	for(auto x = 0; x < mWaveRect.width()-10; x++) {

		int blockPs = 0, blockNg = 0;
		long long accPs = 0.0, accNg = 0.0;
		for(int i = 0; i < step; i++) {

			if(i%2) {
				sampleIndex++;
				continue;
			}

			if((sample = mWaveData[sampleIndex++]) >= 0) {
				accPs += sample;
				blockPs++;
			} else {
				accNg += sample;
				blockNg++;
			}
		}

		y = centre - qFloor((accPs/blockPs) * stepHi);
		painter.drawLine(x,centre, x, y);

		std::cout << x << ": " << y;

		y = centre + qFloor((accNg/blockNg) * -stepLo);
		painter.drawLine(x,centre, x, y);
		std::cout << y << std::endl;


	}

	if(mHoverPosition >= 0) {
		pen.setColor("#8E06A0");
		painter.setPen(pen);
		painter.drawLine(mHoverPosition,0, mHoverPosition, mWaveRect.height());
	}

}

void SWaveform::setWaveData(const signed short* data, long long length) {
	mWaveData = (short*)data;
	mWaveLength = length;
	generateWaveform();
}

void SWaveform::generateWaveform() {
}

void SWaveform::mouseMoveEvent(QMouseEvent* event) {
	if(this->rect().contains(event->pos())) {
		mHoverPosition = event->x();
	} else {
		mHoverPosition = -1;
	}
	update();

}
