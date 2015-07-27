#include "SWindow.hpp"
SWindow::SWindow(QWidget *parent) :
QWidget(parent) {
	setFixedSize(900, 750);
	setWindowTitle("StrangePad");
	setObjectName("mainStrangePad");
	setupUi();
	setupStyles();
}

void SWindow::setupUi() {
	setLayout(&mContainer);
	mContainer.setParent(this);
	mContainer.addWidget(&mHud);
	mHud.setTabShape(QTabWidget::Triangular);
	mHud.setObjectName("hudContainer");
}

void SWindow::addHeadsup(SHud *widget) {
	widget->setParent(&mHud);
	mHud.addTab(widget, widget->getLabel());
}

void SWindow::setupStyles() {
}
