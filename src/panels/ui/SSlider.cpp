#include "SSlider.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <iostream>
SSlider::SSlider(QWidget* parent)
: QAbstractSlider(parent) {
	mGrabbed = false;
	setMinimum(0); setMaximum(128);
	setSingleStep(1);
	setValue(0);
}


void SSlider::paintEvent(QPaintEvent*) {
	QPainter painter(this);

	painter.setRenderHints(QPainter::Antialiasing);

	auto xpos = ((width()-15)/maximum()*value());
	cursor = new QRectF(xpos,0,30,30);


	QRectF pilot(xpos,0, (width()-10)-xpos, 30);
	QRectF highlight(0,0, xpos+30, 30);

	painter.setPen(QPen(QColor("#161616")));
	painter.setBrush(QBrush(QColor("#240128")));
	painter.drawRoundedRect(pilot, 15,15);

	painter.setBrush(QBrush(QColor("#6E047C")));

	QLinearGradient gradient;
        gradient.setStart(highlight.width() / 2, 0);
        gradient.setFinalStop(highlight.width() / 2, highlight.height());

        gradient.setColorAt(0, "#B64FC4");
        gradient.setColorAt(1, "#6E047C");
	painter.setBrush(QBrush(gradient));
	painter.drawRoundedRect(highlight, 15,15);


	if(mGrabbed) {
		painter.setBrush(QBrush(QColor("#D24CE3")));
	} else {
		painter.setBrush(QBrush(QColor("#8E06A0")));
	}
	painter.setPen(QPen(QColor("#52015B")));
	painter.drawEllipse(*cursor);
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
		auto val = ((mouse->x()-15) / ((width())/maximum()));
		setValue(val);
		update();
	}
}


void SSlider::setOrientation(SSlider::Orientation orientation) {
	mOrientation = orientation;
}
