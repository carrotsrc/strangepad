#include "SpSine.hpp"
#include "PanelOverview.hpp"

QWidget *SpSineBuild(const QString & panelId) {
	if(panelId == "overview") {
		return new SpSineOverview();
	}

	return nullptr;
}
