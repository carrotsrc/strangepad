#include <QPushButton>

#include "PanelOverview.hpp"

SpSineOverview::SpSineOverview(QWidget *parent) :
SPad(parent) {
	auto button = new QPushButton("Foobar Strangepad");
	setLayout(&mContainer);
	mContainer.addWidget(button);
}
