#include <math.h>
#include <iostream>
#include "BfSineOsc.hpp"

using namespace RackoonIO;

BfSineOsc::BfSineOsc()
: RackUnit("BfSineOsc") {
	mState = IDLE;
	mBlockSize = 512;
	mFs = 44100;
	mF0 = 220;
	m2Pi = 2*M_PI;
	mX = 0ull;

	mWave = WaveStrategy::Freq;

	addJack("power", JACK_AC);
	addPlug("audio_out");
	mState = IDLE;
}

FeedState BfSineOsc::feed(Jack*) {
	return FEED_OK;
}

void BfSineOsc::setConfig(std::string config, std::string value) {
	if(config == "strategy") {
		if(value == "freq") mWave = Freq;
		else if(value == "nyq") mWave = Nyq;
		else if(value == "0.5 nyq") mWave = HalfNyq;
		else if(value == "0.25 nyq") mWave = QuarterNyq;
		else mWave = Freq;
	}
}

RackState BfSineOsc::init() {
	mOut = getPlug("audio_out")->jack;
	mOut->frames = mBlockSize;
	mState = READY;
	switch(mWave) {
	case Freq: UnitMsg("Freqency Strategy"); break;
	case Nyq: UnitMsg("Nyquist Frequency Strategy"); break;
	case HalfNyq: UnitMsg("Half Nyquist Frequency Strategy"); break;
	case QuarterNyq: UnitMsg("Quarter Nyquist Frequency Strategy"); break;
	}
	UnitMsg("Initialised");

	return RACK_UNIT_OK;
}

RackState BfSineOsc::cycle() {
	if(mState == READY) writeSamples();
	mState = (mOut->feed(mPeriod) == FEED_OK)
		? READY : WAITING;
	return RACK_UNIT_OK;
}

void BfSineOsc::block(RackoonIO::Jack*) {

}

void BfSineOsc::writeSamples() {
	mPeriod = cacheAlloc(1);
	switch(mWave) {
	case Freq: stratFreq(); break;
	case Nyq: stratNyq(); break;
	case HalfNyq: stratHalfNyq(); break;
	case QuarterNyq: stratQuarterNyq(); break;
	}
	mState = WAITING;
}

void BfSineOsc::stratFreq() {
	for(auto i = 0; i < mBlockSize; i++) {
		mPeriod[i] = sin(m2Pi*mF0*mX++/mFs);
	}
}

void BfSineOsc::stratNyq() {
	auto val = 1;
	for(auto i = 0; i < mBlockSize; i++) {
		mPeriod[i++] = val;
		mPeriod[i] = val;

		val *= -1;
	}
}

void BfSineOsc::stratHalfNyq() {
	auto val = 1;
	for(auto i = 0; i < mBlockSize; i++) {
		mPeriod[i++] = val;
		mPeriod[i++] = val;
		mPeriod[i++] = 0;
		mPeriod[i] = 0;

		val *= -1;
	}
}

void BfSineOsc::stratQuarterNyq() {

	// incorrect
	auto val = 1.0f;
	auto valq = 0.707f;
	for(auto i = 0; i < mBlockSize; i++) {
		mPeriod[i++] = 0;
		mPeriod[i++] = 0;

		mPeriod[i++] = valq;
		mPeriod[i++] = valq;

		mPeriod[i++] = val;
		mPeriod[i++] = val;

		mPeriod[i++] = valq;
		mPeriod[i] = valq;
		

		val *= -1;
		valq *= -1;
	}
}
DynamicBuilder(BfSineOsc);