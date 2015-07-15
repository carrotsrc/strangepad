#include <QPushButton>

#include "PanelOverview.hpp"

SpSineOverview::SpSineOverview(QWidget *parent) :
SPad(parent) {
	auto button = new QPushButton("Foobar Strangepad");
	button->setStyleSheet("width: 100px;");
	setLayout(&mContainer);
	mContainer.addWidget(button);
}
