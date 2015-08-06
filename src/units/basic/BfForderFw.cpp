#include "framework/helpers/midi.h"
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
	MidiExport("a1", BfForderFw::midiChangeA1);
	MidiExport("a2", BfForderFw::midiChangeA2);
}

FeedState BfForderFw::feed(Jack *jack) {

	if(mState == WAITING)
		return FEED_WAIT;

	PcmSample *fPeriod;
	mPeriod = cacheAlloc(1);
	auto ptrWrite = mPeriod;
	auto nSamples = jack->numSamples;
	auto a1 = mA1, a2 = mA2, lz = mLeftZ, rz = mRightZ;

	for(auto channel = 0; channel < jack->numChannels; channel++) {

		jack->flush(&fPeriod,channel+1);
		ptrWrite = mPeriod + (channel*nSamples);

		for(auto i = 0; i < nSamples; i++) {
			auto sample = fPeriod[i];

			if(channel == 1) {
				ptrWrite[i] = (a1*sample) + (a2*lz);

				if(mEcho) {
					std:: cout 
						<< sample << "\t"
						<< mLeftZ << "\t" 
						<< mPeriod[i] 
					<< std::endl;
				}

				lz = sample;
			} else {
				ptrWrite[i] = (a1*sample) + (a2*rz);
				rz = sample;
			}
		}
	}
	mLeftZ = lz;
	mRightZ = rz;
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
	if(mOut) {
		mOut->numSamples = 256;
		mOut->numChannels = 2;
	}

	mLeftZ = mRightZ = 0.0f;

	if(mEcho) UnitMsg("Echo enabled");

	mState = READY;
	UnitMsg("Initialised");

	return RACK_UNIT_OK;
}

RackState BfForderFw::cycle() {
	if(mState == READY || mState == PROCESSING) {
		return RACK_UNIT_OK;
	}

	mState = (mOut->feed(mPeriod) == FEED_OK)
		? READY : WAITING;

	return RACK_UNIT_OK;
}

void BfForderFw::block(RackoonIO::Jack*) {

}

void BfForderFw::midiChangeA1(int value) {
	mA1 = Helpers::MidiRoutines::normaliseVelocity64(value);
}

void BfForderFw::midiChangeA2(int value) {
	mA2 = Helpers::MidiRoutines::normaliseVelocity64(value);
}

DynamicBuilder(BfForderFw);
