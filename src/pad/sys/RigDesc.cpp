#include "RigDesc.hpp"
#include <iostream>
void RigDesc::addHud(QString label) {
	mHuds.push_back({
		.label = label,
		.pads = QVector<PadDesc>()
	});
	mCurrentHud = &(mHuds.last());
}

void RigDesc::addPad(QString type, QString label, QString unit) {
	mCurrentHud->pads.push_back({
				.type = type,
				.label = label,
				.unit = unit
			});
}

QVector<HudDesc>::const_iterator RigDesc::begin() const {
	return mHuds.constBegin();
}

QVector<HudDesc>::const_iterator RigDesc::end() const {
	return mHuds.constEnd();
}
