#include "RigDesc.hpp"

void RigDesc::addHud(QString label) {
	huds.insert(label, new HudDesc);
}
