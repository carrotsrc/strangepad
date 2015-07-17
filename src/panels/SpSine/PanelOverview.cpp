#include <QLabel>

#include "PanelOverview.hpp"
#include "../ui/SVIndicator.hpp"

SpSineOverview::SpSineOverview(QWidget *parent) :
SPad(parent) {
	auto widget = new SVIndicator();
	auto label = new QLabel("StrangePad Sine");

	label->setStyleSheet("width: 100px;");

	setLayout(&mContainer);
	mContainer.addWidget(label);
	mContainer.addWidget(widget);
}
