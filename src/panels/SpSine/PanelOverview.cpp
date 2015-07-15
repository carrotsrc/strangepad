#include <QPushButton>
#include "PanelOverview.hpp"

SpSineOverview::SpSineOverview(QWidget *parent) {
	auto button = new QPushButton("Foobar Strangepad");
	setLayout(&mContainer);
	mContainer.addWidget(button);
}
