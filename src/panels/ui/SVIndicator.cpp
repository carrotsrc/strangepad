#include <QPainter>
#include "SVIndicator.hpp"
SVIndicator::SVIndicator(QWidget* parent)
:QWidget(parent) {
	int offset = 0;
	for(int i = 0; i < 9; i++) {
		auto width = 40+(4*i)+((i*i)/2);
		auto top = i*15;
		mLeds.push_back(QRectF(10,top,width,10));
	}
}

void SVIndicator::paintEvent(QPaintEvent*) {

	QPainter painter(this);
	for(auto led : mLeds) {
		painter.drawRoundedRect(led, 5, 5);
	}

}
