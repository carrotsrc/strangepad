#include "ForderFw.hpp"
using namespace RackoonIO;

ForderFw::ForderFw()
: RackUnit("ForderFw") {
	addJack("audio", JACK_SEQ);
	addPlug("audio_out");
	mState = IDLE;
}

FeedState ForderFw::feed(Jack*) {
	return FEED_OK;
}

void ForderFw::setConfig(std::string,std::string) {

}

RackState ForderFw::init() {
	mState = READY;
	UnitMsg("Initialised");

	return RACK_UNIT_OK;
}

RackState ForderFw::cycle() {
	return RACK_UNIT_OK;
}

void ForderFw::block(RackoonIO::Jack*) {

}
