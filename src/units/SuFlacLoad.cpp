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
#include "SuFlacLoad.h"
#define CHUNK_SIZE 0x100000
using namespace RackoonIO;

SuFlacLoad::SuFlacLoad()
: RackUnit(std::string("SuFlacLoad")) {
	addJack("power", JACK_AC);
	addPlug("audio_out");

	MidiExport("pause", SuFlacLoad::midiPause);

	buffer = nullptr;
	workState = IDLE;
	psize = 512;

}

RackoonIO::FeedState SuFlacLoad::feed(RackoonIO::Jack*jack) {
	return FEED_OK;
}

void SuFlacLoad::setConfig(std::string config, std::string value) {
	if(config == "filename") {
		filename = value;
	}
}

void SuFlacLoad::actionNextChunk() {
	period = nullptr;

	while(period == nullptr)
		period = cacheAlloc(1);

	if(count < psize) psize = count;
	memcpy(period, position, psize*sizeof(PcmSample));
	count -= psize;
	position += psize;
	workState = STREAMING;
	notifyProcComplete();
}

void SuFlacLoad::actionLoadFile() {
	UnitMsg("Loading file " << filename);
	file = new SndfileHandle(filename.c_str());

	if(file->error() > 0) {
		UnitMsg("Error occured when loading file `" << filename << "` with error " << file->error());
		workState = ERROR;
		return;
	}


	bufSize = file->frames()<<1;
	count = bufSize;
	if(buffer != nullptr)
		free(buffer);

	buffer = (PcmSample*)calloc(bufSize, sizeof(PcmSample));
	position = buffer;

	while(file->read((float*)position, CHUNK_SIZE) == CHUNK_SIZE) {
		position += CHUNK_SIZE;
	}

	position = buffer;

	onStateChange(PRESTREAM);
	workState = PRESTREAM;
	UnitMsg("Initialised");

	notifyProcComplete();
}

RackoonIO::RackState SuFlacLoad::init() {
	workState = LOADING;
	onStateChange(workState);
	ConcurrentTask(SuFlacLoad::actionLoadFile);
	//EventListener(FramesFinalBuffer, SuFlacLoad::eventFinalBuffer);
	return RACK_UNIT_OK;
}

RackoonIO::RackState SuFlacLoad::cycle() {
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
		jack->frames = psize;
		if(jack->feed(period) == FEED_OK) {
			workState = LOADING_CHUNK;
			ConcurrentTask(SuFlacLoad::actionNextChunk);
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
