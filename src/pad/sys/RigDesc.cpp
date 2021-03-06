#include "RigDesc.hpp"
#include <iostream>

RigDesc::RigDesc() {
	mRackConfig = ".config/pad.cfg";
}
void RigDesc::addHud(QString label) {
	mHuds.push_back({
		.label = label,
		.pads = QVector<PadDesc>()
	});
	mCurrentHud = &(mHuds.last());
}

void RigDesc::addPad(QString collection, QString type, QString label, QString unit) {
	mCurrentHud->pads.push_back({
				.collection = collection,
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

void RigDesc::setRackConfig(QString path) {
	mRackConfig = path;
}

QString RigDesc::getRackConfig() {
	return mRackConfig;
}
void RigDesc::addMidi(QString device, QString code, QString function) {
	mMidi.push_back({
		.device = device,
		.code = code,
		.function = function
	});
	std::cout << "Added midi" << std::endl;
}

const QVector<MidiDesc>& RigDesc::getMidi() const {
	return mMidi;
}