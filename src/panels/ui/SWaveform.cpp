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
	for(auto i = 0; i < mWaveLength;) {

		if(i >= 0) {
			y = centre - qFloor(mWaveData[i] * stepHi);
		} else {
			y = centre + qFloor(mWaveData[i] * stepLo);
		}
		//std::cout << "Data" << mWaveData[i] << std::endl;
		
		painter.drawLine(x,centre, x, y);
		x++;
		i += step;
	}

}

void SWaveform::setWaveData(const signed short* data, long long length) {
	mWaveData = (short*)data;
	mWaveLength = length;
	generateWaveform();
}

void SWaveform::generateWaveform() {
}
