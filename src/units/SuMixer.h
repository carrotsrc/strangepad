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
#include "framework/rack/RackUnit.h"

class SuMixer : public RackoonIO::RackUnit {
	enum WorkState {
		IDLE,
		INIT,
		READY
	};
	PcmSample *periodC1, *periodC2, *mixedPeriod;
	float gainC1, gainC2, peakC1, peakC2;
	WorkState workState;

	short mixerState;

	std::mutex mMut;

public:
	SuMixer();
	RackoonIO::FeedState feed(RackoonIO::Jack*);
	void setConfig(std::string,std::string);

	RackoonIO::RackState init();
	RackoonIO::RackState cycle();
	void block(RackoonIO::Jack*);

	PcmSample getChannelPeak(int channel);

	void midiFade(int);
};
#endif