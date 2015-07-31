#include "SuEncoder.h"
using namespace RackoonIO;
#define PHASE 0

SuEncoder::SuEncoder()
: RackUnit(std::string("SuEncoder")) {
	addJack("audio", JACK_SEQ);

	mPath = "dump.raw";
	mFormat = SuEncoder::Raw;
}

FeedState SuEncoder::feed(Jack *jack) {
	jack->flush(&mPeriod);

	fwrite(mPeriod, sizeof(PcmSample), jack->frames, mFp);
	cacheFree(mPeriod);
	notifyProcComplete();
	return FEED_OK;
}

void SuEncoder::setConfig(std::string config, std::string value) { 
	// configuration sets centre frequency
	if(config == "file") {
		mPath = value;
	} else if(config == "format") {
		if(value == "raw") mFormat = SuEncoder::Raw;
		else mFormat = SuEncoder::Raw;
	}

}

RackState SuEncoder::init() {
	if(!(mFp = fopen(mPath.c_str(), "wb")))
		return RACK_UNIT_FAILURE;

	UnitMsg("Writing to " << mPath);
	switch(mFormat) {
		case SuEncoder::Raw: UnitMsg("Raw format"); break;
	}
	UnitMsg("Initialised");
	return RACK_UNIT_OK;
}

RackState SuEncoder::cycle() {
	return RACK_UNIT_OK;
}

void SuEncoder::block(Jack *jack) {
}

// Make this unit loadable at runtime by defining a builder method
DynamicBuilder(SuEncoder);

