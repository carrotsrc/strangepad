#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include "SPad.hpp"
SPad::SPad(QWidget *parent) :
QWidget(parent) { 
}
void SPad::paintEvent(QPaintEvent *) {
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}



