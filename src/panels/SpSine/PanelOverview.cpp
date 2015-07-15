#include <QPushButton>
#include "PanelOverview.hpp"

SpSineOverview::SpSineOverview(const QString & label, QWidget *parent) {
	auto button = new QButton("Foobar Strangepad");
	setLayout(&mContainer);
	mContainer.addWidget(button);
}
