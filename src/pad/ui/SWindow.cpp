#include "SWindow.hpp"
SWindow::SWindow(QWidget *parent) :
QWidget(parent) {
	setFixedSize(900, 700);
	setWindowTitle("StrangePad");
	setObjectName("mainStrangePad");
	setupUi();
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

