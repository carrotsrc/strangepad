#include <QPainter>
#include <qmath.h>
#include "SWaveform.hpp"
#include <iostream>
SWaveform::SWaveform(QWidget* parent)
: QWidget(parent) {
	mWaveData = nullptr;
}

void SWaveform::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	QPen pen(QColor("#ffffff"));

	QRectF rect(0,0, width(), height());

	painter.setPen(pen);
	painter.setBrush(QColor("#ff0000"));

	if(mWaveData == nullptr) return;
	auto step = mWaveLength/(width()-10);
	if(step%2 != 0)
		step++;
	auto centre = height()/2;

	signed short max = 32768;
	signed short min = -32767;

	auto stepHi = (height()/2.0)/max;
	auto stepLo = (height()/2.0)/min;


	auto x = 0, y = 0;
	auto sampleIndex = 0;
	signed short sample;

	for(auto x = 0; x < width()-10; x++) {

		int blockPs = 0, blockNg = 0;
		long long accPs = 0, accNg = 0;
		for(int i = 0; i < step; i++) {
			if((sample = mWaveData[sampleIndex++]) >= 0) {
				accPs += sample;
				blockPs++;
			} else {
				accNg += sample;
				blockNg++;
			}
		}

		y = centre - qFloor((accPs/blockPs) * stepHi);

		std::cout << y << ",";
		painter.drawLine(x,centre, x, y);

		y = centre + qFloor((accNg/blockNg) * -stepLo);
		std::cout << y << std::endl;;
		painter.drawLine(x,centre, x, y);
		x++;
	}

}

void SWaveform::setWaveData(const signed short* data, long long length) {
	mWaveData = (short*)data;
	mWaveLength = length;
	generateWaveform();
}

void SWaveform::generateWaveform() {
}
