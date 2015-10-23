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
#include "framework/helpers/sound.h"
#include "../msg/ClientMessages.hpp"
#include "SuFlacLoad.h"
#define CHUNK_SIZE 0x100000
using namespace StrangeIO;

SuFlacLoad::SuFlacLoad()
: RackUnit(std::string("SuFlacLoad")) {
	addJack(std::string("power"), JACK_AC, 0);
	addPlug(std::string("audio_out"));

	MidiExport("pause", SuFlacLoad::midiPause);

	buffer = nullptr;
	workState = IDLE;
	psize = 512;
	csize = psize/2;

}

StrangeIO::FeedState SuFlacLoad::feed(StrangeIO::Jack*jack) {
	return FEED_OK;1
}

void SuFlacLoad::setConfig(std::string config, std::string value) {
	if(config == "filename") {
		filename = value;
	}
}

void SuFlacLoad::actionNextChunk() {
	period = cacheAlloc(1);
	auto tp = cacheAlloc(1);

	if(count < psize) psize = count;
	std::copy(position, position+psize, tp);
	Helpers::SoundRoutines::deinterleave2(tp, period, csize);

	cacheFree(tp);

	count -= psize;
	position += psize;
	workState = STREAMING;
}

void SuFlacLoad::actionLoadFile() {
	UnitMsg("Loading file " << filename);
	SndfileHandle file(filename.c_str());

	if(file.error() > 0) {
		UnitMsg("Error occured when loading file `" << filename << "` with error " << file.error());
		workState = ERROR;
		return;
	}


	// Two channels, two datapoints per frame
	bufSize = file.frames()*2;
	count = bufSize;

	if(buffer != nullptr)
		free(buffer);

	buffer = (PcmSample*)calloc(bufSize, sizeof(PcmSample));
	position = buffer;

	while(file.read((PcmSample*)position, CHUNK_SIZE) == CHUNK_SIZE) {
		position += CHUNK_SIZE;
	}

	position = buffer;

	onStateChange(PRESTREAM);
	workState = PRESTREAM;
	UnitMsg("Initialised");
	mSamplesOut = 0;

	notifyProcComplete();
}

StrangeIO::RackState SuFlacLoad::init() {

	addEventListener(SndSamplesOut, [this](std::shared_ptr<EventMessage> msg){
		if(workState < STREAMING)
			return;
		mSamplesOut += SndSamplesOutCast(msg)->numSamples;
	});

	if(filename == "") return RACK_UNIT_OK;

	workState = LOADING;
	onStateChange(workState);
	ConcurrentTask(SuFlacLoad::actionLoadFile);
	mSamplesOut = 0;
	return RACK_UNIT_OK;
}

StrangeIO::RackState SuFlacLoad::cycle() {
	if(workState < READY)
		return RACK_UNIT_OK;
	
	Jack *jack = nullptr;

	switch(workState) {
	case ERROR:
		return RACK_UNIT_FAILURE;

	case READY:
		workState = LOADING_CHUNK;
		onStateChange(STREAMING);
		ConcurrentTask(SuFlacLoad::actionNextChunk);
		break;

	case STREAMING:
		jack = getPlug("audio_out")->jack;
		jack->numChannels = 2;
		jack->numSamples = csize;

		if(jack->feed(period) == FEED_OK) {
			workState = LOADING_CHUNK;
			ConcurrentTask(SuFlacLoad::actionNextChunk);
		} else {
		}
		break;

	case LOADING:
	case PAUSED:
	case LOADING_CHUNK:
	case PRESTREAM:
	case IDLE:
		break;
	}

	return RACK_UNIT_OK;
}

void SuFlacLoad::block(Jack *jack) {
	Jack *out = getPlug("audio_out")->jack;
	workState = PAUSED;

	onStateChange(workState);
	out->block();
}

void SuFlacLoad::midiPause(int code) {
	if(code == 127) {
		if(workState == STREAMING) {
			block(NULL);
		} else
		if(workState == PRESTREAM) {
			workState = READY;
			notifyProcComplete();
			onStateChange(workState);
		} else {
			workState = STREAMING;
			notifyProcComplete();
			onStateChange(workState);
		}
	}
}

void SuFlacLoad::eventFinalBuffer(std::shared_ptr<EventMessage> msg) {
}


/* Exposed Unit */

void SuFlacLoad::setFilename(std::string fname) {
	filename = fname;
}

std::string SuFlacLoad::getFilename() {
	return filename;
}

void SuFlacLoad::cbStateChange(std::weak_ptr<std::function<void(SuFlacLoad::WorkState)>> cb) {
	mStateChangeListeners.push_back(cb);
}

void SuFlacLoad::onStateChange(SuFlacLoad::WorkState state) {
	for(auto wptr : mStateChangeListeners) {
		if(auto cb = wptr.lock()) {
			(*cb)(state);
		}
	}
}

const PcmSample* SuFlacLoad::getSampleData() const {
	return buffer;
}

int SuFlacLoad::getSpc() const {
	// Samples per channel
	return bufSize/2;
}

int SuFlacLoad::getProgress() const {
	return mSamplesOut;
}


