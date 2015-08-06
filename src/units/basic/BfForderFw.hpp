#ifndef __BFFORDERFW_HPP_1438254060__
#define __FORDERFW_HPP_1438254060__
#include "framework/rack/RackUnit.h"
class BfForderFw : public RackoonIO::RackUnit
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
	RackoonIO::FeedState feed(RackoonIO::Jack*);
	void setConfig(std::string,std::string);

	RackoonIO::RackState init();
	RackoonIO::RackState cycle();
	void block(RackoonIO::Jack*);

private:
	WorkState mState;
	PcmSample *mPeriod;
	RackoonIO::Jack *mOut;

	PcmSample mLeftZ, mRightZ;
	float mA1, mA2;
	bool mEcho;

	void midiChangeA1(int value);
	void midiChangeA2(int value);
};

#endif

