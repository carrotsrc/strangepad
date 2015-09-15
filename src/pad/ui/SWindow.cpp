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

void SWindow::bindMidi(const QVector<MidiDesc> & bindings, siomid::midi_handler* handler) {

	for(auto& binding : bindings) {
		if(binding.function == "hud_left") {
			handler->add_binding(binding.device.toStdString(), binding.code.toInt(),
								[this](siomid::msg m) {
									if(!m.v) return;

									auto i = mHud.currentIndex();
									if(i == 0) i = mHud.count();
									i--;
									mHud.setCurrentIndex(i);
									
								} );
		}

		if(binding.function == "hud_right") {
			handler->add_binding(binding.device.toStdString(), binding.code.toInt(),
								[this](siomid::msg m) {
									if(!m.v) return;

									auto i = mHud.currentIndex();
									if(i == mHud.count()-1) i = 0;
									else i++;
									
									mHud.setCurrentIndex(i);
									
								} );
		}
	}
}
