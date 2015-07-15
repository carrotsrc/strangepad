#include <QApplication>
#include <QPushButton>
#include <QFile>
#include "ui/SWindow.hpp"
#include "ui/SHud.hpp"
#include <iostream>
int main(int argc, char **argv)
{
	QApplication app (argc, argv);

	// load style sheet
	QFile qss(".config/strange.qss");
	std::cout << QString(qss.readAll()).toStdString() << std::endl;
	qss.open(QFile::ReadOnly);
	app.setStyleSheet(qss.readAll());

	auto button = new QPushButton();
	button->setText("Hello");

	SWindow window;
	SHud hud("Overview");
	SHud dhud("Focal");

	hud.addWidget(button);
	window.addHeadsup(&hud);
	window.addHeadsup(&dhud);
	window.show();

	return app.exec();
}
