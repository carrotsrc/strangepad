#ifndef __BFSINEOSC_HPP_1438269457__
#define __BFSINEOSC_HPP_1438269457__
#include <atomic>
#include "framework/rack/RackUnit.h"
class BfSineOsc : public StrangeIO::RackUnit
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
	StrangeIO::FeedState feed(StrangeIO::Jack*);
	void setConfig(std::string,std::string);

	StrangeIO::RackState init();
	StrangeIO::RackState cycle();
	void block(StrangeIO::Jack*);

private:
	WorkState mState;
	PcmSample *mPeriod;
	std::atomic<int> mWave;

	int mBlockSize, ///< Period size in frames
	    mFs, ///< Sample rate
	    mF0; ///< Frequencey for freq strat

	float m2Pi; ///< Value of 2PI
	unsigned long long mX;
	StrangeIO::Jack *mOut;

	void writeSamples();
	void stratFreq();
	void stratNyq();
	void stratHalfNyq();
	void stratQuarterNyq();

};
#endif
