#include <QLabel>

#include "PanelOverview.hpp"

SpSineOverview::SpSineOverview(QWidget *parent) :
SPad(parent) {
	auto label = new QLabel("StrangePad Sine");
	label->setStyleSheet("width: 100px;");
	setLayout(&mContainer);
	mContainer.addWidget(label);
}
