#include "SWindow.hpp"
SWindow::SWindow(QWidget *parent) :
QWidget(parent) {
	setFixedSize(900, 500);
	setWindowTitle("StrangePad");
	setLayout(&container);
	container.setParent(this);
	container.addWidget(&hud);
}

void SWindow::addHeadsup(SHud *widget) {
	widget->setParent(&hud);
	hud.addTab(widget, widget->getLabel());
}
