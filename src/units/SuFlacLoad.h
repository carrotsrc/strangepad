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
#ifndef SUFLACLOAD_H
#define SUFLACLOAD_H
#include "framework/rack/RackUnit.h"
#include <sndfile.hh>


class SuFlacLoad : public RackoonIO::RackUnit
{
public:
	enum WorkState {
		IDLE,
		LOADING,
		READY,
		STREAMING,
		PAUSED,
		LOADING_CHUNK,
		PRESTREAM,

		ERROR,
	};

private:
	WorkState workState;

	SndfileHandle *file;
	sf_count_t bufSize, count;

	std::string filename;
	PcmSample *buffer, *position, *period;
	int psize, csize;

	void actionLoadFile();
	void actionNextChunk();

	void eventFinalBuffer(std::shared_ptr<RackoonIO::EventMessage>);
	std::vector<std::weak_ptr<std::function<void(SuFlacLoad::WorkState) > > > mStateChangeListeners;

	void onStateChange(SuFlacLoad::WorkState state);

public:
	SuFlacLoad();
	RackoonIO::FeedState feed(RackoonIO::Jack*);
	void setConfig(std::string,std::string);

	RackoonIO::RackState init();
	RackoonIO::RackState cycle();
	void block(RackoonIO::Jack*);
	RackoonIO::FeedState feed();

	void midiPause(int);
	void midiLoad(int);

	/* Unit Exposure */
	void setFilename(std::string);
	std::string getFilename();
	void cbStateChange(std::weak_ptr<std::function<void(SuFlacLoad::WorkState)>>);
	const PcmSample* getSampleData() const;
	int getSpc() const;
};


#ifndef SUFLACLOAD_GUARDED
DynamicBuilder(SuFlacLoad);
#else
using SuflCbStateChange = std::function<void(SuFlacLoad::WorkState)>;
#endif

#endif
