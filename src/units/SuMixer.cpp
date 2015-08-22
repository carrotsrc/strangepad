/* Copyright 2015 Charlie Fyvie-Gauld
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published 
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "SuMixer.h"
#include "framework/helpers/midi.h"
#define MIXER_C1_BUF 1
#define MIXER_C2_BUF 2

#define MIXER_C1_ACT 4
#define MIXER_C2_ACT 8

#define MIXER_BUFFER 64

#define C1_FULL (mixerState&MIXER_C1_BUF)
#define C2_FULL (mixerState&MIXER_C2_BUF)
#define MIXER_FULL (mixerState&MIXER_BUFFER)

#define C1 1
#define C2 2
using namespace StrangeIO;
SuMixer::SuMixer()
: RackUnit(std::string("SuMixer")) {
	addJack("channel_1", JACK_SEQ, C1);
	addJack("channel_2", JACK_SEQ, C2);
	addPlug("audio_out");

	mixedPeriod = periodC1 = periodC2 = nullptr;

	faderC1 = faderC2 = 1.0f;
	peakC1 = peakC2 = 0.0f;
	gainC1 = gainC2 = 0.5f;
	gainMaster = 0.5f;

	mixerState = MIXER_C2_ACT|MIXER_C1_ACT;
	MidiExport("channelFade", SuMixer::midiFade);

	midiExportMethod("channel1Gain", [this](int value) {
				this->gainC1 = Helpers::MidiRoutines::normaliseVelocity128(value);
				onGainChange(Channel1, value);

			});

	midiExportMethod("channel2Gain", [this](int value) {
				this->gainC2 = Helpers::MidiRoutines::normaliseVelocity128(value);
				onGainChange(Channel2, value);
			});

	midiExportMethod("masterGain", [this](int value) {
				this->gainMaster = Helpers::MidiRoutines::normaliseVelocity128(value);
				onGainChange(Master, value);
			});
}


FeedState SuMixer::feed(Jack *jack) {

	mOut = getPlug("audio_out")->jack;
	mOut->numSamples = jack->numSamples;
	mOut->numChannels = jack->numChannels;

	if(MIXER_FULL) {
		return FEED_WAIT;
	}

	// could be stale data here
	if(jack->id == C1) {

		// Activate Channel 1 if inactive
		if(mixerState&MIXER_C1_ACT)
			mixerState^=MIXER_C1_ACT;


		// if channel 2 is off
		if(mixerState&MIXER_C2_ACT) {

			if(mWaiting) return FEED_WAIT;

			jack->flush(&waitPeriod, 1);

			auto peak = (waitPeriod[0]*gainC1);
			auto gain = gainC1;

			for(auto i = 0; i < mOut->numSamples*2; i++) {
				waitPeriod[i] = waitPeriod[i] * gain;
				peak = waitPeriod[i] > peak ? waitPeriod[i] : peak;
				waitPeriod[i] = waitPeriod[i] * gainMaster;
			}

			mMut.lock();
				peakC1 = peak;
			mMut.unlock();

			mWaiting = true;
			return FEED_OK;
		}

		// Channel 1 is full
		if( C1_FULL ) {
			return FEED_WAIT;
		} else {
			// buffer channel 1
			jack->flush(&periodC1, 1);
			mixerState ^= MIXER_C1_BUF;
		}

	} else {

		if(mixerState&MIXER_C2_ACT)
			mixerState^=MIXER_C2_ACT;


		if(mixerState&MIXER_C1_ACT) {

			if(mWaiting) return FEED_WAIT;

			jack->flush(&waitPeriod, 1);

			auto peak = (waitPeriod[0]*gainC2);
			auto gain = gainC2;

			for(auto i = 0; i < mOut->numSamples*2; i++) {
				waitPeriod[i] = waitPeriod[i] * gain;
				peak = waitPeriod[i] > peak ? waitPeriod[i] : peak;
				waitPeriod[i] = waitPeriod[i] * gainMaster;
			}

			mMut.lock();
				peakC2 = peak;
			mMut.unlock();
			mWaiting = true;

			return FEED_OK;

		}

		if( C2_FULL ) {
			return FEED_WAIT;
		} else {
			jack->flush(&periodC2, 1);
			mixerState ^= MIXER_C2_BUF;
		}
	}

	if(!C1_FULL || !C2_FULL) {
		return FEED_OK;
	} else {
		// mix and feed if both buffers are full
		auto pc1 = periodC1[0]*gainC1*faderC1;
		auto pc2 = periodC2[0]*gainC2*faderC2;
		
		while(!mixedPeriod)
			mixedPeriod = cacheAlloc(1);

		auto totalSamples = jack->numSamples * 2;
		for(int i = 0; i < totalSamples; i++) {
			auto c1 = periodC1[i] * gainC1 * faderC2;
			auto c2 = periodC2[i] * gainC2 * faderC1;
			pc1 = c1 > pc1 ? c1 : pc1;
			pc2 = c2 > pc2 ? c2 : pc2;
			mixedPeriod[i] = (c1+c2)*gainMaster;
		}
		mMut.lock();
			peakC1 = pc1;
			peakC2 = pc2;
		mMut.unlock();


		cacheFree(periodC1);
		cacheFree(periodC2);
		mixerState ^= (MIXER_BUFFER ^ MIXER_C1_BUF ^ MIXER_C2_BUF );
	}

	return FEED_OK;
}
RackState SuMixer::init() {
	workState = READY;
	UnitMsg("Initialised");
	return RACK_UNIT_OK;
}

RackState SuMixer::cycle() {

	if(MIXER_FULL) {
		if(mOut->feed(mixedPeriod) == FEED_OK) {
			mixerState ^= MIXER_BUFFER;
			mixedPeriod = nullptr;
		}
	} else if(mWaiting) {
		if(mOut->feed(waitPeriod) == FEED_OK) {
			mWaiting = false;
		}
	}

	return RACK_UNIT_OK;
}

void SuMixer::setConfig(std::string config, std::string value) {

}

void SuMixer::block(Jack *jack) {
	Jack *out = getPlug("audio_out")->jack;
	UnitMsg("Block from " << jack->name);
	if(jack->name == "channel_1") {
		mixerState ^= MIXER_C1_ACT;
	} else {
		mixerState ^= MIXER_C2_ACT;
	}

	if(mixerState&MIXER_C1_ACT && mixerState&MIXER_C2_ACT)
		out->block();

}

void SuMixer::midiFade(int value) {
	if(value == 64) {
		faderC1 = faderC2 = 1.0;
	}
	else
	if(value > 64) {
		// right channel open
		// left channel closing
		faderC1 = 1.0;
		faderC2 = (1-(100-((127-(float)value)/63)*100)/100);
	} else
	if(value < 64) {
		// right channel closing
		// left channel open
		faderC1 = (((float)value/64)*100)/100;
		faderC2 = 1.0;
	}
	onGainChange(Fader, value);
}

PcmSample SuMixer::getChannelPeak(int channel) {
	auto p = 0.0f;
	mMut.lock();
	if(channel) {
		p = peakC2;
	} else {
		p = peakC1;
	}
	mMut.unlock();
	return p;
}

void SuMixer::cbGainChange(std::weak_ptr<std::function<void(SuMixer::GainType, int)>> cb) {
	mGainListeners.push_back(cb);
}

void SuMixer::onGainChange(SuMixer::GainType type, int value) {
	for(auto wptr : mGainListeners) {
		if(auto cb = wptr.lock()) {
			(*cb)(type, value);
		}
	}
}
// Make this unit loadable at runtime by defining a builder method
DynamicBuilder(SuMixer);
