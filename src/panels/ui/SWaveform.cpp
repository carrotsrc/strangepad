#include <QPainter>
#include "SWaveform.hpp"

SWaveform::SWaveform(QWidget* parent)
: QWidget(parent) {

}

void SWaveform::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	QPen pen(QColor("#ffffff"));

	QRectF rect(0,0, width(), height());

	painter.setPen(pen);
	painter.setBrush(QColor("#ff0000"));

	painter.drawRect(rect);
}

void SWaveform::setData(const short* data) {

}
