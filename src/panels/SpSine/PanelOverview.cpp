#include <QLabel>

#include "PanelOverview.hpp"
#include "../ui/SVIndicator.hpp"

#include <iostream>
SpSineOverview::SpSineOverview(QWidget *parent) :
SPad(parent) {
	auto widget = new SVIndicator();
	auto widgetA = new SVIndicator();
	widgetA->setOrientation(SVIndicator::Left);
	auto label = new QLabel("StrangePad Sine");


	setLayout(&mContainer);
	auto hbox = new QHBoxLayout();
	hbox->addWidget(widgetA);
	hbox->addWidget(label);
	hbox->addWidget(widget);

	mContainer.addLayout(hbox);
}
