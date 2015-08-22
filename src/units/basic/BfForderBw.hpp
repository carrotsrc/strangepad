#ifndef __BFFORDERBW_HPP_1438972200__
#define __BFFORDERBW_HPP_1438972200__
#include "framework/rack/RackUnit.h"
class BfForderBw : public StrangeIO::RackUnit
{
public:
	enum WorkState {
		IDLE,
		INIT,
		READY,
		WAITING,
		PROCESSING
	};

	BfForderBw();
	StrangeIO::FeedState feed(StrangeIO::Jack*);
	void setConfig(std::string,std::string);

	StrangeIO::RackState init();
	StrangeIO::RackState cycle();
	void block(StrangeIO::Jack*);

private:
	WorkState mState;
	PcmSample *mPeriod;
	StrangeIO::Jack *mOut;

	PcmSample mLeftZ, mRightZ;
	float mA0, mB1;
	bool mEcho;

	void midiChangeA0(int value);
	void midiChangeB1(int value);
};
#endif
