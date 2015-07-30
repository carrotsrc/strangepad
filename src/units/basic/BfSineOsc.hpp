#ifndef __BFSINEOSC_HPP_1438269457__
#define __BFSINEOSC_HPP_1438269457__
#include <atomic>
#include "framework/rack/RackUnit.h"
class BfSineOsc : public RackoonIO::RackUnit
{
public:
	enum WorkState {
		IDLE,
		INIT,
		READY,
		WAITING,
	};

	enum WaveStrategy {
		Freq,
		Nyq,
		HalfNyq,
		QuarterNyq
	};

	BfSineOsc();
	RackoonIO::FeedState feed(RackoonIO::Jack*);
	void setConfig(std::string,std::string);

	RackoonIO::RackState init();
	RackoonIO::RackState cycle();
	void block(RackoonIO::Jack*);

private:
	WorkState mState;
	PcmSample *mPeriod;
	std::atomic<int> mWave;

	int mBlockSize, ///< Period size in frames
	    mFs, ///< Sample rate
	    mF0; ///< Frequencey for freq strat

	float m2Pi; ///< Value of 2PI
	unsigned long long mX;
	RackoonIO::Jack *mOut;

	void writeSamples();
	void stratFreq();
	void stratNyq();
	void stratHalfNyq();
	void stratQuarterNyq();

};
#endif
