#ifndef __SUENCODER_HPP_1438361786__
#define __SUENCODER_HPP_1438361786__
#include "framework/rack/RackUnit.h"
/** Sine wave generator unit
 *
 * Produces a sine wave of a particular freq.
 * 
 * - Demonstrates exporting methods
 *   for binding to midi controllers
 *
 * For details on the different API methods,
 * see RuAlsa or RuImpulse
 */
class SuEncoder : public StrangeIO::RackUnit
{
public:
	enum WorkState {
		IDLE,
		INIT,
		READY,
		WAITING,
	};

	enum Format {
		Raw
	};

	SuEncoder();
	StrangeIO::FeedState feed(StrangeIO::Jack*);
	void setConfig(std::string,std::string);

	StrangeIO::RackState init();
	StrangeIO::RackState cycle();
	void block(StrangeIO::Jack*);

private:
	WorkState workState;
	std::string mPath;
	FILE *mFp;
	PcmSample *mPeriod;
	int mFormat;
};
#endif

