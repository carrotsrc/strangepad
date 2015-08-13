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
#ifndef SuMixer_H
#define SuMixer_H
#include <atomic>
#include "framework/rack/RackUnit.h"

class SuMixer : public StrangeIO::RackUnit {
	enum WorkState {
		IDLE,
		INIT,
		READY
	};
	PcmSample *periodC1, *periodC2, *mixedPeriod, *waitPeriod;
	
	float faderC1, faderC2, 
		  peakC1, peakC2,
		  gainC1, gainC2,
		  gainMaster;

	StrangeIO::Jack *mOut;
	WorkState workState;

	std::atomic<short> mixerState;
	std::atomic<bool> mWaiting;

	std::mutex mMut;

public:
	SuMixer();
	StrangeIO::FeedState feed(StrangeIO::Jack*);
	void setConfig(std::string,std::string);

	StrangeIO::RackState init();
	StrangeIO::RackState cycle();
	void block(StrangeIO::Jack*);

	PcmSample getChannelPeak(int channel);

	void midiFade(int);
};
#endif
