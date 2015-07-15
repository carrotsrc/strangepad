#include "SHud.hpp"

SHud::SHud(const QString & label, QWidget *parent) :
QWidget(parent) {
	setLayout(&mContainer);
	mLabel = label;
}

void SHud::addWidget(QWidget *widget) {
	mContainer.addWidget(widget);
}

const QString SHud::getLabel() {
	return mLabel;
}
