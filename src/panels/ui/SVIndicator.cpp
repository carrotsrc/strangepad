#include <QPainter>
#include <qmath.h>
#include <iostream>
#include "SVIndicator.hpp"
SVIndicator::SVIndicator(QWidget* parent)
:QWidget(parent) {
	setMaximumWidth(120);
	high = new QColor("#F92F2F"); highOff = new QColor("#520404");
	mid = new QColor("#F7F72E"); midOff = new QColor("#545400");
	low = new QColor("#70F42E"); lowOff = new QColor("#174600");
	setOrientation(Orientation::Right);
	mValue = 0.0f;
}

SVIndicator::~SVIndicator() {
	mLeds.clear();
}

void SVIndicator::paintEvent(QPaintEvent*) {

	if(height() != mHeightTracker) {
		setOrientation(mOrientation);
	}

	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing);
	painter.setPen(QPen(QColor(33,33,33)));
	auto region = 0;
	auto mark = 10 - 10.0f*mValue;
	auto hBrush = QBrush(*highOff);
	auto mBrush = QBrush(*midOff);
	auto lBrush = QBrush(*lowOff);
	for(auto led : mLeds) {

		if( mark == 0) {
			hBrush = QBrush(*high);
			mBrush = QBrush(*mid);
			lBrush = QBrush(*low);

			if(region == 0) painter.setBrush(hBrush);
			if(region >= 2 && region < 4) painter.setBrush(mBrush);
			if(region >= 4) painter.setBrush(lBrush);
		}

		if(region == 0) painter.setBrush(hBrush);
		if(region == 2) painter.setBrush(mBrush);
		if(region == 4) painter.setBrush(lBrush);


		painter.drawRoundedRect(led, 5, 5);
		region++;
		mark--;
	}

}

void SVIndicator::setOrientation(SVIndicator::Orientation orientation) {
	mOrientation = orientation;

	mHeightTracker = height();
	auto h = (mHeightTracker-40)/10;
	auto off = h+5;

	switch(mOrientation) {
	case Orientation::Right:
		mLeds.clear();
		for(int i = 0; i < 10; i++) {
			auto width = 40+(4*i)+((i*i)/2);
			auto top = i*off;
			mLeds.push_back(QRectF(0,top,width,h));
		}
		break;

	default:
		mLeds.clear();
		for(int i = 0; i < 10; i++) {
			auto width = 40+(4*i)+((i*i)/2);
			auto top = i*off;
			mLeds.push_back(QRectF(120-width,top,width,h));
		}
		break;
	}


}

void SVIndicator::setValue(float value) {
	mValue = value;
	emit update();
}
