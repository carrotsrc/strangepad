#include "framework/events/EventMessage.h"
#define SndSamplesOut 10
#define SndSamplesOutCast(ptr)(static_cast<SndSamplesOutMsg*>(ptr.get()))
class SndSamplesOutMsg : public StrangeIO::EventMessage {
public:
	int numSamples;
};
