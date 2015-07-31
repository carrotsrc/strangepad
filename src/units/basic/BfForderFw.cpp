#include "BfForderFw.hpp"
using namespace RackoonIO;

BfForderFw::BfForderFw()
: RackUnit("BfForderFw") {
	addJack("audio", JACK_SEQ);
	addPlug("audio_out");
	mOut = nullptr;

	mState = IDLE;
	mEcho = false;

	mA1 = mA2 = 0.5f;
	
}

FeedState BfForderFw::feed(Jack *jack) {

	if(mState == WAITING)
		return FEED_WAIT;

	PcmSample *fPeriod;
	jack->flush(&fPeriod);
	mPeriod = cacheAlloc(1);
	auto nSamples = jack->frames;
	for(auto i = 0; i < nSamples; i++) {
		auto sample = fPeriod[i];

		if(i%2 == 0) {
			mPeriod[i] = (mA1*sample) + (mA2*mLeftZ);
			if(mEcho) {
			std:: cout 
				<< sample << "\t"
				<< mLeftZ << "\t" 
				<< mPeriod[i] << std::endl;
			mLeftZ = sample;
			}
		} else {
			mPeriod[i] = (mA1*sample) + (mA2*mRightZ);
			mRightZ = sample;
		}
	}
	cacheFree(fPeriod);
	if(mOut == nullptr) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		notifyProcComplete();
		cacheFree(mPeriod);
	}
	else
		mState = WAITING;

	return FEED_OK;
}

void BfForderFw::setConfig(std::string config, std::string value) {
	if(config == "a1") {
		mA1 = atof(value.c_str());
	} else if(config == "a2") {
		mA2 = atof(value.c_str());
	} else if(config == "echo" && value == "true") {
		mEcho = true;
	}
}

RackState BfForderFw::init() {
	mOut = getPlug("audio_out")->jack;
	if(mOut) mOut->frames = 512;
	mLeftZ = mRightZ = 0.0f;

	if(mEcho) UnitMsg("Echo enabled");

	mState = READY;
	UnitMsg("Initialised");
	notifyProcComplete();

	return RACK_UNIT_OK;
}

RackState BfForderFw::cycle() {
	if(mState == READY) {
		return RACK_UNIT_OK;
	}

	mState = (mOut->feed(mPeriod) == FEED_OK)
		? READY : WAITING;

	return RACK_UNIT_OK;
}

void BfForderFw::block(RackoonIO::Jack*) {

}

DynamicBuilder(BfForderFw);
