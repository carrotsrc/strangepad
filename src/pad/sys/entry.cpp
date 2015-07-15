#include <QApplication>
#include <QPushButton>
#include "ui/SWindow.hpp"
#include "ui/SHud.hpp"
int main(int argc, char **argv)
{
	QApplication app (argc, argv);
	auto button = new QPushButton();
	button->setText("Hello");

	SWindow window;
	SHud hud("Overview");

	hud.addWidget(button);
	window.addHeadsup(&hud);
	window.show();

	return app.exec();
}
