#include "SKnob.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <qmath.h>
#include <math.h>
#include <iostream>
SKnob::SKnob(QWidget *parent)
: QAbstractSlider(parent) {
	setRange(0,127);
	setValue(64);
	mKnobContainer = new QRectF(0,0,40,40);
	mKnobHighlight = new QRectF(0,0,50,50);
}

void SKnob::paintEvent(QPaintEvent*) {

	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing);

	auto a = (int)(width()/2); auto b = (int)(height()/2);
	auto step = 240.0/maximum();
	auto theta = (value() * step)+150;

	QPen pen("#52015B");
	painter.setBrush(QColor("#5FA4FF"));
	pen.setWidth(0);
	painter.setPen(pen);

	mKnobHighlight->moveTo(a-25,b-25);
	painter.drawPie(*mKnobHighlight, 150*-16, ((theta-150)*-16));

	mKnobContainer->moveTo(a-20,b-20);

	pen.setWidth(3);
	painter.setBrush(QColor("#8E06A0"));
	auto x1 = (int) a + 20*std::cos(theta * (M_PI/180.0));
	auto y1 = (int) b + 20*std::sin(theta * (M_PI/180.0));

	painter.setPen(pen);
	painter.drawEllipse(*mKnobContainer);

	pen.setWidth(2);
	pen.setColor(QColor("#A841B6"));
	painter.setPen(pen);
	QLine line(a,b,x1,y1);
	painter.drawLine(line);
}

void SKnob::mousePressEvent(QMouseEvent *mouse) {
	auto x = mouse->x(); auto y = mouse->y();
	if(x > mKnobContainer->x() && x < mKnobContainer->x() + mKnobContainer->width()
	&& y > mKnobContainer->y() && y < mKnobContainer->y() + mKnobContainer->height()) {
		mGrabbed = true;
		mOldY = mouse->y();
		update();
	}
}

void SKnob::mouseReleaseEvent(QMouseEvent*) {
	if(mGrabbed) { 
		mGrabbed = false;
		std::cout << "Value: " << value() << std::endl;
		update();
	}
}

void SKnob::mouseMoveEvent(QMouseEvent *mouse) {
	if(mGrabbed) {
		auto y = mouse->y();
		auto val = value();

		if(y > mOldY) {
			val = (int) val - ((y - mOldY));
		} else { 
			val = (int) val + ((mOldY-y));
		}

		mOldY = y;
		setValue(val);
		update();
	}
}

