#include "BfForderFw.hpp"
using namespace RackoonIO;

BfForderFw::BfForderFw()
: RackUnit("BfForderFw") {
	addJack("audio", JACK_SEQ);
	addPlug("audio_out");
	mState = IDLE;
}

FeedState BfForderFw::feed(Jack*) {
	return FEED_OK;
}

void BfForderFw::setConfig(std::string,std::string) {

}

RackState BfForderFw::init() {
	mState = READY;
	UnitMsg("Initialised");

	return RACK_UNIT_OK;
}

RackState BfForderFw::cycle() {
	return RACK_UNIT_OK;
}

void BfForderFw::block(RackoonIO::Jack*) {

}
