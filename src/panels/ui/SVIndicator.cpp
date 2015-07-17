#include <QPainter>
#include "SVIndicator.hpp"
SVIndicator::SVIndicator(QWidget* parent)
:QWidget(parent) {

	high = new QColor(249,47,47);
	mid = new QColor(247,247,46);
	low = new QColor(112,244,46);

	int offset = 0;
	for(int i = 0; i < 9; i++) {
		auto width = 40+(4*i)+((i*i)/2);
		auto top = i*15;
		mLeds.push_back(QRectF(10,top,width,10));
	}
}

SVIndicator::~SVIndicator() {
	mLeds.clear();
}
void SVIndicator::paintEvent(QPaintEvent*) {

	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing);
	painter.setPen(QPen(QColor(33,33,33)));
	int region = 0;

	for(auto led : mLeds) {
		if(region == 0) painter.setBrush(QBrush(*high));
		if(region == 3) painter.setBrush(QBrush(*mid));
		if(region == 6) painter.setBrush(QBrush(*low));

		painter.drawRoundedRect(led, 5, 5);
		region++;
	}

}
