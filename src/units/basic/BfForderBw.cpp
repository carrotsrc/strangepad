#include "framework/helpers/midi.h"
#include "BfForderBw.hpp"

using namespace StrangeIO;

BfForderBw::BfForderBw()
: RackUnit("BfForderBw") {
	addJack("audio", JACK_SEQ);
	addPlug("audio_out");
	mOut = nullptr;

	mState = IDLE;
	mEcho = false;

	mA0 = mB1 = 0.5f;
	MidiExport("a0", BfForderBw::midiChangeA0);
	MidiExport("b1", BfForderBw::midiChangeB1);
}

FeedState BfForderBw::feed(Jack *jack) {

	if(mState == WAITING)
		return FEED_WAIT;

	PcmSample *fPeriod;
	mPeriod = cacheAlloc(1);
	auto ptrWrite = mPeriod;
	auto nSamples = jack->numSamples;
	auto a0 = mA0, b1 = mB1, lz = mLeftZ, rz = mRightZ;

	for(auto channel = 0; channel < jack->numChannels; channel++) {

		jack->flush(&fPeriod,channel+1);
		ptrWrite = mPeriod + (channel*nSamples);

		for(auto i = 0; i < nSamples; i++) {
			auto sample = fPeriod[i];

			if(channel == 1) {
				lz = (a0*sample) - (b1*lz);
				ptrWrite[i] = lz;

				if(mEcho) {
					std:: cout 
						<< sample << "\t"
						<< mLeftZ << "\t" 
						<< mPeriod[i] 
					<< std::endl;
				}

			} else {
				rz = (a0*sample) - (b1*rz);
				ptrWrite[i] = rz;
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

void BfForderBw::setConfig(std::string config, std::string value) {
	if(config == "a0") {
		mA0 = atof(value.c_str());
	} else if(config == "b1") {
		mB1 = atof(value.c_str());
	} else if(config == "echo" && value == "true") {
		mEcho = true;
	}
}

RackState BfForderBw::init() {
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

RackState BfForderBw::cycle() {
	if(mState == READY || mState == PROCESSING) {
		return RACK_UNIT_OK;
	}

	mState = (mOut->feed(mPeriod) == FEED_OK)
		? READY : WAITING;

	return RACK_UNIT_OK;
}

void BfForderBw::block(StrangeIO::Jack*) {

}

void BfForderBw::midiChangeA0(int value) {
	mA0 = Helpers::MidiRoutines::normaliseVelocity64(value);
	UnitMsg("a0: " << mA0);
}

void BfForderBw::midiChangeB1(int value) {
	mB1 = Helpers::MidiRoutines::normaliseVelocity64(value);
	UnitMsg("b1: " << mB1);
}

DynamicBuilder(BfForderBw);
