#include "SSlider.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <iostream>
SSlider::SSlider(QWidget* parent)
: QAbstractSlider(parent) {
	mGrabbed = false;
	setMinimum(0); setMaximum(127);
	setSingleStep(1);
	setValue(0);
	setBarSize(Width::Large);
}


void SSlider::paintEvent(QPaintEvent*) {

	// This whole function is suboptimal
	QPainter painter(this);

	painter.setRenderHints(QPainter::Antialiasing);

	//auto xpos = ((width()-mHalfWidth)/maximum()*value());
	//cursor = new QRectF(xpos,0,mWidth,mWidth);

	QRectF pilot(0,0, width(), mControlSize);
	painter.setPen(QPen(QColor("#161616")));
	painter.setBrush(QBrush(QColor("#240128")));
	painter.drawRoundedRect(pilot, mHalfWidth,mHalfWidth);
/*
	QRectF pilot(xpos,0, width()-xpos-(mWidth*(1.75f)), mWidth);
	QRectF highlight(0,0, xpos+mWidth, mWidth);
	QRectF textBlock(0,mWidth+5, width()-10, 15);

	painter.setPen(QPen(QColor("#161616")));
	painter.setBrush(QBrush(QColor("#240128")));
	painter.drawRoundedRect(pilot, mHalfWidth,mHalfWidth);

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
*/
//	painter.drawText(textBlock, Qt::AlignCenter, QString::number(value()));
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
	std::cout << width() << ", " << height() << std::endl;
	return QSize(width(), mControlSize);
}
