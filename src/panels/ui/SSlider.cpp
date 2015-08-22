#include "SSlider.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <qmath.h>
#include <iostream>
SSlider::SSlider(QWidget* parent)
: QAbstractSlider(parent) {
	mGrabbed = false;
	setMinimum(0); setMaximum(127);
	setSingleStep(1);
	setStart(0);
	setBarSize(Width::Large);
}


void SSlider::paintEvent(QPaintEvent*) {

	// This whole function is suboptimal
	QPainter painter(this);

	painter.setRenderHints(QPainter::Antialiasing);

	auto widthActual = ((width()-20-30)/maximum())*maximum();
	mWidthActual = widthActual;
	auto xActual = 10+(width()-20-widthActual)/2;
	mXActual = xActual;
	auto jump = widthActual/maximum();
	mJump = jump;

	auto xpos = (jump*value())+xActual;
	cursor = new QRectF(xpos,0,mControlSize,mControlSize);
	QRectF pilot(xActual,0, widthActual+mControlSize, mControlSize);
	painter.setPen(QPen(QColor("#161616")));
	painter.setBrush(QBrush(QColor("#240128")));
	painter.drawRoundedRect(pilot, mHalfWidth,mHalfWidth);
	QRectF highlight;

	auto startX = mStart*mJump+mXActual;
	if(xpos == startX) {
		highlight.setRect(0,0,0,0);
	} else if(xpos > startX) {
		highlight.setRect(startX,0, xpos+mControlSize-startX, mControlSize);
	} else {
		highlight.setRect(xpos,0, startX-(xpos)+mControlSize, mControlSize);
	}

	painter.setBrush(QBrush(QColor("#6E047C")));

	QLinearGradient gradient;
	gradient.setStart(highlight.width() / 2, 0);
	gradient.setFinalStop(highlight.width() / 2, highlight.height());

	gradient.setColorAt(0, "#B64FC4");
	gradient.setColorAt(1, "#6E047C");
	painter.setBrush(QBrush(gradient));
	painter.drawRoundedRect(highlight, mHalfWidth,mHalfWidth);

	if(mGrabbed) {
		painter.setBrush(QBrush(QColor("#D24CE3")));
	} else {
		painter.setBrush(QBrush(QColor("#8E06A0")));
	}

	painter.setPen(QPen(QColor("#52015B")));
	painter.drawEllipse(*cursor);
	painter.setPen(QPen(QColor("#B1B7E6")));
}

void SSlider::mousePressEvent(QMouseEvent *mouse) {
	auto x = mouse->x(); auto y = mouse->y();
	if(x > cursor->x() && x < cursor->x() + cursor->width()
	&& y > cursor->y() && y < cursor->y() + cursor->height()) {
		mGrabbed = true;
		update();
	}
}

void SSlider::mouseReleaseEvent(QMouseEvent*) {
	if(mGrabbed) { 
		mGrabbed = false;
		update();
	}
}

void SSlider::mouseMoveEvent(QMouseEvent *mouse) {
	if(mGrabbed) {
		auto val = (mouse->x()-mXActual-mHalfWidth) / mJump;
		setValue(val);
		update();
	}
}


void SSlider::setOrientation(SSlider::Orientation orientation) {
	mOrientation = orientation;
}

void SSlider::setBarSize(SSlider::Width width) {
	mSize = width;
	switch(mSize) {
	case Width::Small: mControlSize = 18; break;
	case Width::Medium: mControlSize = 24; break;
	case Width::Large: mControlSize = 30; break;
	}

	mHalfWidth = mControlSize / 2;
}

QSize SSlider::sizeHint() const {
	return QSize(width(), mControlSize);
}

void SSlider::setStart(int start) {
	mStart = start;
	setValue(mStart);
}

int SSlider::start() {
	return mStart;
}
