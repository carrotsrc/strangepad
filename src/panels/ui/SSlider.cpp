#include "SSlider.hpp"

SSlider::SSlider(QWidget* parent)
: QAbstractSlider(parent) {

}


void SSlider::paintEvent(QPaintEvent*) {

}

void SSlider::setOrientation(SSlider::Orientation orientation) {
	mOrientation = orientation;
}
