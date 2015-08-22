#ifndef __BFFORDERFW_HPP_1438254060__
#define __FORDERFW_HPP_1438254060__
#include "framework/rack/RackUnit.h"
class BfForderFw : public StrangeIO::RackUnit
{
public:
	enum WorkState {
		IDLE,
		INIT,
		READY,
		WAITING,
		PROCESSING
	};

	BfForderFw();
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
	float mA1, mA2;
	bool mEcho;

	void midiChangeA1(int value);
	void midiChangeA2(int value);
};

#endif

