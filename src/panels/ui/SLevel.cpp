#include <QPainter>
#include <qmath.h>
#include <iostream>
#include "SLevel.hpp"
SLevel::SLevel(QWidget* parent)
:QWidget(parent) {
	setMaximumWidth(120);
	highOn = "#F92F2F"; highOff = "#520404";
	midOn = "#F7F72E"; midOff = "#545400";
	lowOn = "#70F42E"; lowOff = "#174600";

	setOrientation(Orientation::Right);
	mValue = 0.0f;
}

SLevel::~SLevel() {
	mLeds.clear();
}

void SLevel::paintEvent(QPaintEvent*) {
	

	if(height() != mHeightTracker) {
		setOrientation(mOrientation);
	}

	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing);


	painter.setPen(QPen(QColor(33,33,33)));
	auto region = 0;
	mMut.lock();
	auto mark = mValue;
	mMut.unlock();

	auto hBrush = QBrush(QColor(highOff));
	auto mBrush = QBrush(QColor(midOff));
	auto lBrush = QBrush(QColor(lowOff));
		
	for(auto led : mLeds) {
		
		if( mark == 0) {
			if(region == 0) painter.setBrush(hBrush);
			hBrush.setColor(QColor(highOn));
			mBrush.setColor(QColor(midOn));
			lBrush.setColor(QColor(lowOn));

			if(region >= 2 && region < 4) painter.setBrush(mBrush);
			if(region >= 4) painter.setBrush(lBrush);

		}

		if(region == 0) painter.setBrush(hBrush);
		if(region == 2) painter.setBrush(mBrush);
		if(region == 4) painter.setBrush(lBrush);


		painter.drawRoundedRect(led, 5, 5);
		region++;

		if(mark) mark--;
	}

}

void SLevel::setOrientation(SLevel::Orientation orientation) {


	mOrientation = orientation;

	mHeightTracker = height();
	mWidthTracker = width();

	auto h = qFloor((mHeightTracker-50)/10);
	auto off = h+5;
	auto w = mWidthTracker-80;

	switch(mOrientation) {
	case Orientation::Right:
		mLeds.clear();
		for(int i = 0; i < 10; i++) {
			auto width = w+(4*i)+((i*i)/2);
			auto top = i*off;
			mLeds.push_back(QRectF(0,top,width,h));
		}
		break;

	default:
		mLeds.clear();
		for(int i = 0; i < 10; i++) {
			auto width = w+(4*i)+((i*i)/2);
			auto top = i*off;
			mLeds.push_back(QRectF(mWidthTracker-width,top,width,h));
		}
		break;
	}


}

void SLevel::setValue(float value) {
	if(value < 0)
		value = value * -1.0f;

	mMut.lock();
	mValue = qFloor(10 - 10.0f*value);;
	mMut.unlock();
}

QSize SLevel::sizeHint() const {
	std::cout << "SLevel size: " << width() << "," << height() << std::endl;
	return QSize(width(), height());
}
