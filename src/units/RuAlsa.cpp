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
#include "RuAlsa.h"
#include "framework/events/FrameworkMessages.h"
#include "framework/helpers/sound.h"
using namespace StrangeIO;
using namespace StrangeIO::Buffers;


static void pcm_trigger_callback(snd_async_handler_t *);
/** Set the default values */
RuAlsa::RuAlsa()
: RackUnit("RuAlsa") {
	addJack("audio", JACK_SEQ);
	workState = IDLE;
	sampleRate = 44100;
	maxPeriods = 4;
	bufSize = 2048;
	frameBuffer = nullptr;
	//fp = fopen("dumpx.raw", "wb");
}

/** Method that is called when there is data waiting to be fed into the unit
 *
 * Here we decide whether to accept the data and store it in the buffer
 * or respond with a FEED_WAIT
 */
StrangeIO::FeedState RuAlsa::feed(StrangeIO::Jack *jack) {
	PcmSample *period;
	auto periodSize = jack->numChannels*jack->numSamples;
	// here the buffer has reached capacity
	if(frameBuffer->hasCapacity(periodSize) == DelayBuffer::WAIT) {
		return FEED_WAIT; // so response with a WAIT
	}


	// If we're here then the buffer has room
	jack->flush(&period, 1);
	bufLock.lock();
	if(workState == PAUSED) {
		UnitMsg("Unpaused");
		workState = STREAMING;
	}
	auto tp = cacheAlloc(1);
	Helpers::SoundRoutines::interleave2(period, tp, jack->numSamples);

	frameBuffer->supply(tp, periodSize);

	bufLock.unlock();

	cacheFree(tp);
	cacheFree(period);

	notifyProcComplete();

	return FEED_OK; // We've accepted the period
}
/** Method for setting configurations
 *
 * This unit accepts:
 * 	- unit_buffer : The size of the buffer (in frames)
 * 	- max_periods : The maximum periods that can be held by ASLA
 */
void RuAlsa::setConfig(std::string config, std::string value) {
	if(config == "unit_buffer") {
		bufSize = (snd_pcm_uframes_t)atoi(value.c_str());
		frameBuffer = new DelayBuffer(bufSize);
	} else if(config == "max_periods") {
		maxPeriods = atoi(value.c_str());
	}
}

/** This is am outsourced method for flushing the delay buffer
 *
 * This is the task that writes the delay buffer into ALSA
 */
void RuAlsa::actionFlushBuffer() {
	bufLock.lock();
	snd_pcm_uframes_t nFrames;
	auto size = frameBuffer->getLoad();
	if(size == 0) {
		workState = STREAMING;
		bufLock.unlock();
		return;
	}
	auto frames = frameBuffer->flush();
	if((nFrames = snd_pcm_writei(handle, frames, (size>>1))) != (size>>1)) {
		if(nFrames == -EPIPE) {
			if(workState != PAUSED)
				std::cerr << "Underrun occurred" << std::endl;
			snd_pcm_recover(handle, nFrames, 0);

		} else {
			std::cerr << "Screwed: Code[" << (signed int)nFrames << "]" << std::endl;
			std::cerr << snd_strerror(nFrames) << std::endl;
			snd_pcm_recover(handle, nFrames, 0);
		}
	}

	//fwrite(frames, sizeof(PcmSample), size, fp);
	bufLock.unlock();
	auto msg = createMessage(SndSamplesOut);
	SndSamplesOutCast(msg)->numSamples = static_cast<int>(nFrames);
	std::cout << nFrames << std::endl;
	addEvent(std::move(msg));
	notifyProcComplete();
	if(workState == PAUSED)
		return;

	/* once it's done, set the unit back to streaming
	 * so the buffer continued to fill up
	 */
	workState = STREAMING;
}


/** Intitialise the Unit
 *
 * This is called on the warm-up cycle.
 * We don't initialise the unit here,
 * but outsource the task
 */
StrangeIO::RackState RuAlsa::init() {

	/* Set to INIT because the unit
	 * won't be ready by the end
	 * of the method. It will still
	 * be initialising
	 */
	workState = INIT;

	/* Here the task it outsourced to the threadpool
	 * which means the rack cycle is not being blocked
	 */

	ConcurrentTask(RuAlsa::actionInitAlsa);
	/* ^^^^ that is a macro which expands to this:
	 *
	 * outsource(std::bind(&RuAlsa::actionInitAlsa, this));
	 */

	return RACK_UNIT_OK;
}

/** The method on the rack cycle (After the warm-up cycle)
 *
 * It is important to keep track of the state since it
 * will be changed by parallel tasks in another thread
 */
StrangeIO::RackState RuAlsa::cycle() {

	if(workState == STREAMING) {
		auto currentLevel = snd_pcm_avail_update(handle);
		// Check to see if ALSA has reached the threshold
		if(frameBuffer->getLoad() > 0 && currentLevel > triggerLevel) {
			// ALSA is running out of frames! Trigger a flush
			workState = FLUSHING; // Change state

			// outsource the flushing task
			outsource(std::bind(&RuAlsa::actionFlushBuffer, this));
		}

		return RACK_UNIT_OK;
	}

	if(workState == PRIMING && frameBuffer->getLoad() >= (fPeriod<<1)) {
		/* Here the delay buffer has been primed
		 * and ready to start feeding to alsa
		 */
		workState = STREAMING;
	}

	if(workState < READY)
		return RACK_UNIT_OK;

	if(workState == READY) {
		/* The unit has been initialised
		 * (by the other thread) and so
		 * it is time to start priming
		 * the delay buffer.
		 */
		workState = PRIMING;
	}


	return RACK_UNIT_OK;
}

void RuAlsa::block(Jack *jack) {
	workState = PAUSED;
	UnitMsg("Paused");
}

void RuAlsa::triggerAction() {
	mSigCondition.notify_one();
}

/*
 * Single use thread that blocks waiting
 * for the condition variable to be switched
 * so it can send the task for flushing the buffer.
 *
 * Although it could flush the buffer from in here.
 */
void RuAlsa::blockWaitSignal() {
	std::unique_lock<std::mutex> ul(mSigMutex);

	while(1) {
		mSigCondition.wait(ul);
			if(!mThreadRunning) {
				ul.unlock();
				break;
			}
			outsource(std::bind(&RuAlsa::actionFlushBuffer, this));

	}
}

/* This is a signal handler. ALSA uses SIGIO to tell the unit that the
 * output buffer is running low and needs to be refilled. This function
 * is executed when the signal is received. The signal is async and is
 * sent to the main process thread. It is important that there are no heap
 * allocations that occur in the callstack from this function because
 * if a heap allocation in the main process is interrupted by the
 * signal, and another heap allocation occurs somewhere from the signal
 * handler there will be a deadlock on __lll_lock_wait_private.
 *
 * No:
 * [m/c/re]alloc()
 * free()
 * new
 * delete
 */
static void pcm_trigger_callback(snd_async_handler_t *cb) {
	auto callback = (std::function<void(void)>*)snd_async_handler_get_callback_private(cb);
	(*callback)();
}

/** Intialise ALSA
 */
void RuAlsa::actionInitAlsa() {
	snd_pcm_hw_params_t *hw_params;
	int err, dir = 0;


	if(!mThreadRunning) {
		mSigThread = new std::thread(&RuAlsa::blockWaitSignal, this);
		mThreadRunning = true;
	}
	if ((err = snd_pcm_open (&handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		UnitMsg("cannot open audio device `default` - "
			<< snd_strerror(err));
		return;
	}

	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		UnitMsg("cannot allocated hardware param struct - "
			<< snd_strerror(err));
		return;
	}


	if ((err = snd_pcm_hw_params_any (handle, hw_params)) < 0) {
		UnitMsg("cannot init hardware param struct - "
			<< snd_strerror(err));
		return;
	}

	if ((err = snd_pcm_hw_params_set_access (handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		UnitMsg("cannot set access type - "
			<< snd_strerror(err));
		return;
	}

	if ((err = snd_pcm_hw_params_set_format (handle, hw_params, SND_PCM_FORMAT_FLOAT_LE)) < 0) {
		UnitMsg("cannot set format - "
			<< snd_strerror(err));
		return;
	}


	if ((err = snd_pcm_hw_params_set_rate_near (handle, hw_params, &sampleRate, &dir)) < 0) {
		UnitMsg("cannot set sample rate - "
			<< snd_strerror(err));
	}


	if ((err = snd_pcm_hw_params_set_channels (handle, hw_params, 2)) < 0) {
		UnitMsg("cannot set channels - "
			<< snd_strerror(err));
		return;
	}


	auto minPeriodSize = 0ul;
	if ((err = snd_pcm_hw_params_get_period_size_min(hw_params, &minPeriodSize, &dir)) < 0) {
		UnitMsg("cannot get period size - "
			<< snd_strerror(err));
		return;
	}

	if ((err = snd_pcm_hw_params_set_period_size(handle, hw_params, minPeriodSize, dir)) < 0) {
		UnitMsg("cannot set period size - "
			<< snd_strerror(err));
		return;
	}

	if ((err = snd_pcm_hw_params_set_periods_max(handle, hw_params, &maxPeriods, &dir)) < 0) {
		UnitMsg("cannot set periods - "
			<< snd_strerror(err));
		return;
	}

	//if ((err = snd_pcm_hw_params_set_periods(handle, hw_params, maxPeriods, dir)) < 0) {
	if ((err = snd_pcm_hw_params_set_periods(handle, hw_params, 2, dir)) < 0) {
		UnitMsg("cannot set periods - "
			<< snd_strerror(err));
		return;
	}

	if ((err = snd_pcm_hw_params (handle, hw_params)) < 0) {
		UnitMsg("cannot set parameters - "
			<< snd_strerror(err));
		return;
	}

	snd_pcm_hw_params_free (hw_params);

	if ((err = snd_pcm_prepare (handle)) < 0) {
		UnitMsg("cannot prepare audio interface - "
			<< snd_strerror(err));
		return;
	}

	if ((err = snd_pcm_hw_params_get_period_size (hw_params, &fPeriod, &dir)) < 0) {
		UnitMsg("cannot get period size - "
			<< snd_strerror(err));
	}

	UnitMsg("Period size: " << fPeriod);

	snd_pcm_uframes_t bsize;
	if ((err = snd_pcm_hw_params_get_buffer_size (hw_params, &bsize)) < 0) {
		UnitMsg("cannot get sample rate - "
			<< snd_strerror(err));
	}

	UnitMsg("Buffer Size: " << bsize);

	if ((err = snd_pcm_hw_params_get_rate (hw_params, &sampleRate, &dir)) < 0) {
		UnitMsg("cannot get sample rate - "
			<< snd_strerror(err));
	}

	UnitMsg("Fs: " << sampleRate);

	triggerLevel = snd_pcm_avail_update(handle) - (fPeriod<<1);

	if(frameBuffer == nullptr)
		frameBuffer = new Buffers::DelayBuffer(bufSize);

	auto *func = new std::function<void(void)>(std::bind(&RuAlsa::triggerAction, this));
	snd_async_add_pcm_handler(&cb, handle, &pcm_trigger_callback, (void*)func);
	UnitMsg("Initialised");
	notifyProcComplete();
	workState = READY;
}

DynamicBuilder(RuAlsa);
